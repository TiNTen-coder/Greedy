#include "OPTS/graph_part.hpp"
#include "OPTS/huawei_parser.hpp"
#include "OPTS/json_dumper.hpp"
#include "OPTS/logger_config.hpp"

#include <boost/describe/enum_to_string.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <numeric>

namespace opts {
namespace greedy {

TimeDiagram construct_time_schedule(ScheduleData &schedule,
                                     opts::base_config conf) {
    TimeDiagram time_schedule(schedule, schedule.proc_num);

    auto D = schedule.get_top_vertices();

    LOG_INFO << "D updated";

    std::vector<std::size_t> proc_time_vector;

    switch (conf._class) {
    case input_class::class_general:
        proc_time_vector = std::vector<std::size_t>(schedule.task_num);
        std::fill(proc_time_vector.begin(), proc_time_vector.end(), 1);
        break;
    default:
        proc_time_vector = schedule.get_proc_times_vector();
    }

    std::vector<std::size_t> partitioning;

    if (extra_criteria::CR == conf.criteria) {
        LOG_INFO << "CR criteria";

        size_t proc1 = 0;
        size_t group_size = 0;
        for (size_t proc2 = 1; proc2 < schedule.proc_num; ++proc2) {
            if (schedule.long_transmition(proc1, proc2) != -1) {
                group_size = proc2 - 1;
                break;
            }
        }
        if (group_size) {
            LOG_INFO << "CR2 criteria";
            size_t task_num = schedule.task_num, proc_num = schedule.proc_num;
            std::vector<std::size_t> labels;
            uint64_t edgecut;
            auto csr = adjcy2CSR(schedule.graph);
            for (uint64_t ufactor = 30; ufactor < 30000; ufactor += 5) {
                std::tie(labels, edgecut) = part_graph(
                    csr, proc_num / group_size, ufactor, proc_time_vector);
                if (edgecut / (double)schedule.edges < conf.CR2_bound) {
                    break;
                }
            }

            std::vector<std::vector<size_t>> labels2(proc_num / group_size);
            std::map<size_t, size_t> new_nums;
            for (size_t proc_group = 0; proc_group < proc_num / group_size;
                 ++proc_group) {
                std::vector<std::pair<size_t, size_t>> edge_vec;
                size_t vertices = 0;
                for (size_t curr_task = 0; curr_task < task_num; ++curr_task) {
                    size_t curr_proc = labels[curr_task];
                    if (curr_proc != proc_group) {
                        continue;
                    }
                    new_nums[curr_task] = vertices;
                    vertices++;

                    for (auto [frst_chld, lst_chld] =
                             boost::in_edges(curr_task, schedule.graph);
                         frst_chld != lst_chld; ++frst_chld) {
                        size_t parent_task =
                            boost::source(*frst_chld, schedule.graph);
                        size_t parent_proc = labels[parent_task];
                        if (parent_proc == curr_proc) {
                            edge_vec.push_back(
                                {new_nums[parent_task], new_nums[curr_task]});
                        }
                    }
                }

                auto proc_graph =
                    ScheduleData::Graph(edge_vec.begin(), edge_vec.end(), vertices);
                std::vector<std::size_t> curr_task_weights(vertices);
                for (size_t i = 0; i < vertices; ++i) {
                    curr_task_weights[i] = 1;
                }
                auto csr2 = adjcy2CSR(proc_graph);
                std::tie(labels2[proc_group], edgecut) =
                    part_graph(csr2, group_size, 30,
                               curr_task_weights);
            }
            partitioning.resize(task_num);
            for (size_t curr_task = 0; curr_task < task_num; ++curr_task) {
                partitioning[curr_task] =
                    labels[curr_task] * group_size +
                    labels2[labels[curr_task]][new_nums[curr_task]];
            }
        } else {
            auto csr = adjcy2CSR(schedule.graph);
            uint64_t edgecut;
            for (uint64_t ufactor = 30; ufactor < 30000; ufactor += 5) {
                std::tie(partitioning, edgecut) = part_graph(
                    csr, schedule.proc_num, ufactor, proc_time_vector);
                if (edgecut / (double)schedule.edges < conf.CR_bound) {
                    break;
                }
            }
        }
    }

    if ((conf._class == input_class::class_general) &&
        (conf.criteria == extra_criteria::CR)) {
        partitioning =
            local_partition_optimization(partitioning, schedule, conf);
    }

    std::size_t it_counter = 0;
    std::size_t it_counter_max = schedule.task_num;
    double last_ratio = it_counter / (double)it_counter_max;

    BOOST_LOG_NAMED_SCOPE("algo");
    while (!D.empty()) {
        ScheduleData::Task chosen_task;
        chosen_task = schedule.GC1(D);

        LOG_TRACE << "GC1 chosen " << chosen_task;
        ScheduleData::Proc chosen_proc = partitioning[chosen_task];
        time_schedule.add_task(chosen_task, chosen_proc);

        schedule.remove_vertex(chosen_task);
        D = schedule.progress_top_vertices(D, chosen_task);
        if (it_counter % 100 == 0) {
            auto ratio = it_counter / (double)it_counter_max;
            if (ratio - last_ratio > 0.01) {
                LOG_DEBUG << "Progress: " << ratio * 100 << "%";
                LOG_DEBUG << "CR: " << time_schedule.calculate_CR()
                          << "; CR2: " << time_schedule.calculate_CR2();
                last_ratio = ratio;
            }
        }
        ++it_counter;
    }
    LOG_INFO << "schedule calculated";
    return time_schedule;
}

TimeDiagram greedy_EDFBase_heuristic(ScheduleData &sched, opts::base_config conf) {
    BOOST_LOG_NAMED_SCOPE("greedy_EDFBase_heuristic");
    std::size_t right_border = 0;
    std::unordered_set<ScheduleData::Task> leaf_nodes;

    ScheduleData::Graph &gr = sched.get_graph();

    std::vector<std::size_t> proc_time_vector;

    switch (conf._class) {
    case input_class::class_general:
        proc_time_vector = std::vector<std::size_t>(sched.task_num);
        std::fill(proc_time_vector.begin(), proc_time_vector.end(), 1);
        break;
    default:
        proc_time_vector = sched.get_proc_times_vector();
    }

    std::vector<std::size_t> partitioning;
    auto csr = adjcy2CSR(sched.graph);
    if (conf.criteria == extra_criteria::CR) {
        LOG_INFO << "CR criteria";

        size_t proc1 = 0;
        size_t group_size = 0;
        for (size_t proc2 = 1; proc2 < sched.proc_num; ++proc2) {
            if (sched.long_transmition(proc1, proc2) != -1) {
                group_size = proc2 - 1;
                break;
            }
        }
        if (group_size) {
            LOG_INFO << "CR2 criteria";
            size_t task_num = sched.task_num, proc_num = sched.proc_num;
            std::vector<std::size_t> labels;
            uint64_t edgecut;
            auto csr = adjcy2CSR(sched.graph);
            for (uint64_t ufactor = 30; ufactor < 30000; ufactor += 5) {
                std::tie(labels, edgecut) = part_graph(
                    csr, proc_num / group_size, ufactor, proc_time_vector);
                if (edgecut / (double)sched.edges < conf.CR2_bound) {
                    break;
                }
            }

            std::vector<std::vector<size_t>> labels2(proc_num / group_size);
            std::map<size_t, size_t> new_nums;
            for (size_t proc_group = 0; proc_group < proc_num / group_size;
                 ++proc_group) {
                std::vector<std::pair<size_t, size_t>> edge_vec;
                size_t vertices = 0;
                for (size_t curr_task = 0; curr_task < task_num; ++curr_task) {
                    size_t curr_proc = labels[curr_task];
                    if (curr_proc != proc_group) {
                        continue;
                    }
                    new_nums[curr_task] = vertices;
                    vertices++;

                    for (auto [frst_chld, lst_chld] =
                             boost::in_edges(curr_task, sched.graph);
                         frst_chld != lst_chld; ++frst_chld) {
                        size_t parent_task =
                            boost::source(*frst_chld, sched.graph);
                        size_t parent_proc = labels[parent_task];
                        if (parent_proc == curr_proc) {
                            edge_vec.push_back(
                                {new_nums[parent_task], new_nums[curr_task]});
                        }
                    }
                }

                auto proc_graph =
                    ScheduleData::Graph(edge_vec.begin(), edge_vec.end(), vertices);
                std::vector<std::size_t> curr_task_weights(vertices);
                for (size_t i = 0; i < vertices; ++i) {
                    curr_task_weights[i] = 1;
                }
                auto csr2 = adjcy2CSR(proc_graph);
                std::tie(labels2[proc_group], edgecut) =
                    part_graph(csr2, group_size, 30, // 30 is ufactor
                               curr_task_weights);
            }
            partitioning.resize(task_num);
            for (size_t curr_task = 0; curr_task < task_num; ++curr_task) {
                partitioning[curr_task] =
                    labels[curr_task] * group_size +
                    labels2[labels[curr_task]][new_nums[curr_task]];
            }
        } else {
            uint64_t edgecut;
            for (uint64_t ufactor = 30; ufactor < 30000; ufactor += 5) {
                // ЗДЕСЬ ЗАДАЧИ РАСКИДЫВАЮТСЯ ПО ПРОЦЕССОРАМ
                std::tie(partitioning, edgecut) =
                    part_graph(csr, sched.proc_num, ufactor, proc_time_vector);
                if (edgecut / (double)sched.edges < conf.CR_bound) {
                    break;
                }
            }
        }
    }

    if (conf._class == input_class::class_general) {
        LOG_INFO << "balancing";

        partitioning = local_partition_optimization(partitioning, sched, conf);
    }

    LOG_INFO << "Generated partitioning!";

    // https://stackoverflow.com/questions/63041285/boost-graph-algorithm-to-traverse-a-tree-down-to-single-leaf
    for (ScheduleData::Task cur = 0; cur < sched.task_num; ++cur) {
        if (0 == boost::out_degree(cur, gr)) {
            auto [_, successful] = leaf_nodes.insert(cur);
            if (!successful) {
                throw std::runtime_error("Cannot add vertex!");
            }
            gr[cur].deadline = right_border;
        }
    }

    LOG_INFO << "Got first leaf nodes!";

    while (!leaf_nodes.empty()) {
        std::unordered_set<ScheduleData::Task> new_set_of_leaves(leaf_nodes);
        for (const ScheduleData::Task leaf : leaf_nodes) {
            new_set_of_leaves.erase(leaf);
            for (auto [from_it, to_it] = boost::in_edges(leaf, gr);
                 from_it != to_it; ++from_it) {
                ScheduleData::Task task = boost::source(*from_it, gr);
                bool has_real_children = false;
                for (auto [frst_chld, lst_chld] = boost::out_edges(task, gr);
                     frst_chld != lst_chld; ++frst_chld) {
                    auto chld = boost::target(*frst_chld, gr);
                    if (gr[chld].deadline == 1) {
                        has_real_children = true;
                        break;
                    }
                }
                if (!has_real_children && gr[task].deadline == 1) {
                    new_set_of_leaves.insert(task);
                }
            }
        }

        leaf_nodes = std::move(new_set_of_leaves);

        for (const ScheduleData::Task leaf : leaf_nodes) {
            std::vector<ScheduleData::Task> targets;

            for (auto [frst_deadline, last_deadline] =
                     boost::out_edges(leaf, gr);
                 frst_deadline != last_deadline; ++frst_deadline) {
                targets.push_back(boost::target(*frst_deadline, gr));
            }
            //std::cout << "Здесь зашли в лямбду" << std::endl;
            ScheduleData::Task earliest_deadline = *std::min_element(
                targets.begin(), targets.end(),
                [&gr, &sched, &partitioning, &leaf](const ScheduleData::Task &a, const ScheduleData::Task &b) {
                    //std::cout << sched.get_task_time(partitioning[a], a) << " " << sched.tran_times(partitioning[leaf], partitioning[a]) << std::endl;
                    //std::cout << sched.get_task_time(partitioning[b], b) << " " << sched.tran_times(partitioning[leaf], partitioning[b]) << std::endl;
                    //std::cout << std::endl;
                    return gr[a].deadline - sched.get_task_time(partitioning[a], a) -
                     sched.tran_times(partitioning[leaf], partitioning[a]) < gr[b].deadline -
                      sched.get_task_time(partitioning[b], b) - sched.tran_times(partitioning[leaf],
                       partitioning[b]);
                    
                    //return gr[a].deadline < gr[b].deadline;
                });
            /*
            std::cout << "Дедлайны потомков" << std::endl;
            for (auto i : targets) {
                std::cout << gr[i].deadline << " ";
            }
            std::cout << std::endl;
            std::cout << "Процессор будущего листа Время работы задачи Время межпроцессорный передачи" << std::endl;
            std::cout << partitioning[leaf] << " " << sched.get_task_time(partitioning[leaf], leaf) << " " << sched.tran_times(partitioning[leaf],
                                    partitioning[earliest_deadline]) << std::endl;
            */
            gr[leaf].deadline =
                gr[earliest_deadline].deadline -
                sched.get_task_time(partitioning[earliest_deadline], earliest_deadline) -
                sched.tran_times(partitioning[leaf],
                                    partitioning[earliest_deadline]); // Учитывается время межпроцессорной передачи данных
            
            //std::cout << "Назначенный директивный срок" << std::endl;
            //std::cout << gr[leaf].deadline << std::endl;
        }
    }

    LOG_INFO << "All deadlines set up";

    TimeDiagram res(sched, sched.proc_num);

    std::vector<ScheduleData::Task> order(sched.task_num);
    std::iota(order.begin(), order.end(), 0);

    std::sort(order.begin(), order.end(),
              [&gr](const ScheduleData::Task &fr, const ScheduleData::Task &scnd) {
                  return gr[fr].deadline < gr[scnd].deadline;
              });

    LOG_INFO << "Got order";

    std::size_t it_counter = 0;
    std::size_t it_counter_max = sched.task_num;
    double last_ratio = it_counter / (double)it_counter_max;

    for (const ScheduleData::Task &cur : order) {
        ScheduleData::Proc chosen_proc = partitioning[cur];
        // гарантируется соблюдение зависимости по данным за счет 
        // рассмотрения работ в порядке возрастания дир. сроков
        res.add_task(cur, chosen_proc);

        if (it_counter % 100 == 0) {
            auto ratio = it_counter / (double)it_counter_max;
            if (ratio - last_ratio > 0.01) {
                LOG_DEBUG << "Progress: " << ratio * 100 << "%"
                          << "; time = " << res.get_time();
                last_ratio = ratio;
            }
        }
        ++it_counter;
    }

    LOG_INFO << "Time schedule constructed; time = " << res.get_time();

    return res;
}

TimeDiagram greedy_EDFFollow_heuristic(ScheduleData &sched, opts::base_config conf) {
    BOOST_LOG_NAMED_SCOPE("greedy_EDFFollow_heuristic");
    std::size_t right_border = 0;
    std::unordered_set<ScheduleData::Task> leaf_nodes;

    ScheduleData::Graph &gr = sched.get_graph();

    std::vector<std::size_t> proc_time_vector;

    switch (conf._class) {
    case input_class::class_general:
        proc_time_vector = std::vector<std::size_t>(sched.task_num);
        std::fill(proc_time_vector.begin(), proc_time_vector.end(), 1);
        break;
    default:
        proc_time_vector = sched.get_proc_times_vector();
    }

    std::vector<std::size_t> partitioning;
    auto csr = adjcy2CSR(sched.graph);
    if (conf.criteria == extra_criteria::CR) {
        LOG_INFO << "CR criteria";

        size_t proc1 = 0;
        size_t group_size = 0;
        for (size_t proc2 = 1; proc2 < sched.proc_num; ++proc2) {
            if (sched.long_transmition(proc1, proc2) != -1) {
                group_size = proc2 - 1;
                break;
            }
        }
        if (group_size) {
            LOG_INFO << "CR2 criteria";
            size_t task_num = sched.task_num, proc_num = sched.proc_num;
            std::vector<std::size_t> labels;
            uint64_t edgecut;
            auto csr = adjcy2CSR(sched.graph);
            for (uint64_t ufactor = 30; ufactor < 30000; ufactor += 5) {
                std::tie(labels, edgecut) = part_graph(
                    csr, proc_num / group_size, ufactor, proc_time_vector);
                if (edgecut / (double)sched.edges < conf.CR2_bound) {
                    break;
                }
            }

            std::vector<std::vector<size_t>> labels2(proc_num / group_size);
            std::map<size_t, size_t> new_nums;
            for (size_t proc_group = 0; proc_group < proc_num / group_size;
                 ++proc_group) {
                std::vector<std::pair<size_t, size_t>> edge_vec;
                size_t vertices = 0;
                for (size_t curr_task = 0; curr_task < task_num; ++curr_task) {
                    size_t curr_proc = labels[curr_task];
                    if (curr_proc != proc_group) {
                        continue;
                    }
                    new_nums[curr_task] = vertices;
                    vertices++;

                    for (auto [frst_chld, lst_chld] =
                             boost::in_edges(curr_task, sched.graph);
                         frst_chld != lst_chld; ++frst_chld) {
                        size_t parent_task =
                            boost::source(*frst_chld, sched.graph);
                        size_t parent_proc = labels[parent_task];
                        if (parent_proc == curr_proc) {
                            edge_vec.push_back(
                                {new_nums[parent_task], new_nums[curr_task]});
                        }
                    }
                }

                auto proc_graph =
                    ScheduleData::Graph(edge_vec.begin(), edge_vec.end(), vertices);
                std::vector<std::size_t> curr_task_weights(vertices);
                for (size_t i = 0; i < vertices; ++i) {
                    curr_task_weights[i] = 1;
                }
                auto csr2 = adjcy2CSR(proc_graph);
                std::tie(labels2[proc_group], edgecut) =
                    part_graph(csr2, group_size, 30, // 30 is ufactor
                               curr_task_weights);
            }
            partitioning.resize(task_num);
            for (size_t curr_task = 0; curr_task < task_num; ++curr_task) {
                partitioning[curr_task] =
                    labels[curr_task] * group_size +
                    labels2[labels[curr_task]][new_nums[curr_task]];
            }
        } else {
            uint64_t edgecut;
            for (uint64_t ufactor = 30; ufactor < 30000; ufactor += 5) {
                std::tie(partitioning, edgecut) =
                    part_graph(csr, sched.proc_num, ufactor, proc_time_vector);
                if (edgecut / (double)sched.edges < conf.CR_bound) {
                    break;
                }
            }
        }
    }

    if (conf._class == input_class::class_general) {
        LOG_INFO << "balancing";

        partitioning = local_partition_optimization(partitioning, sched, conf);
    }

    LOG_INFO << "Generated partitioning!";

    // https://stackoverflow.com/questions/63041285/boost-graph-algorithm-to-traverse-a-tree-down-to-single-leaf
    for (ScheduleData::Task cur = 0; cur < sched.task_num; ++cur) {
        if (0 == boost::out_degree(cur, gr)) {
            auto [_, successful] = leaf_nodes.insert(cur);
            if (!successful) {
                throw std::runtime_error("Cannot add vertex!");
            }
            gr[cur].deadline = right_border;
        }
    }

    LOG_INFO << "Got first leaf nodes!";

    while (!leaf_nodes.empty()) {
        std::unordered_set<ScheduleData::Task> new_set_of_leaves(leaf_nodes);
        for (const ScheduleData::Task leaf : leaf_nodes) {
            new_set_of_leaves.erase(leaf);
            for (auto [from_it, to_it] = boost::in_edges(leaf, gr);
                 from_it != to_it; ++from_it) {
                ScheduleData::Task task = boost::source(*from_it, gr);
                gr[task].children_on_diff_processes[partitioning[task]] = {};
                bool has_real_children = false;
                for (auto [frst_chld, lst_chld] = boost::out_edges(task, gr);
                     frst_chld != lst_chld; ++frst_chld) {
                    auto chld = boost::target(*frst_chld, gr);

                    gr[task].children_on_diff_processes[partitioning[task]].insert(
                        gr[chld].children_on_diff_processes[partitioning[chld]].begin(),
                        gr[chld].children_on_diff_processes[partitioning[chld]].end()
                    );
                    gr[task].children_on_diff_processes[partitioning[task]].insert(chld);
                    gr[task].ds_abs_min = std::fmin(gr[task].ds_abs_min, std::abs(gr[chld].deadline));
                    gr[task].summary_task_duration += sched.get_task_time(partitioning[chld], chld);


                    if (gr[chld].deadline == 1) {
                        has_real_children = true;
                        break;
                    }
                }
                if (!has_real_children && gr[task].deadline == 1) {
                    new_set_of_leaves.insert(task);
                }
            }
        }

        leaf_nodes = std::move(new_set_of_leaves);

        for (const ScheduleData::Task leaf : leaf_nodes) {
            std::vector<ScheduleData::Task> targets;

            for (auto [frst_deadline, last_deadline] =
                     boost::out_edges(leaf, gr);
                 frst_deadline != last_deadline; ++frst_deadline) {
                targets.push_back(boost::target(*frst_deadline, gr));
            }

            ScheduleData::Task earliest_deadline = *std::min_element(
                targets.begin(), targets.end(),
                [&gr](const ScheduleData::Task &a, const ScheduleData::Task &b) {
                    return gr[a].deadline < gr[b].deadline;
                });
            
            gr[leaf].deadline = -std::fmax(
                std::abs(gr[earliest_deadline].deadline -
                sched.get_task_time(partitioning[leaf], leaf) -
                sched.tran_times(partitioning[leaf],
                                    partitioning[earliest_deadline])), // Учитывается время межпроцессорной передачи данных
                gr[leaf].ds_abs_min + gr[leaf].summary_task_duration
            );
                
        }
        /*
        std::unordered_map<ScheduleData::Task, long long> nodes_on_the_same_proc;

        for (const ScheduleData::Task leaf : copy_leaf_nodes) {

            ScheduleData::Proc leaf_proc = partitioning[leaf];
                for (auto [frst_chld, lst_chld] = boost::out_edges(leaf, gr);
                     frst_chld != lst_chld; ++frst_chld) {
                    auto chld = boost::target(*frst_chld, gr);
                    std::cout << gr[chld].deadline << std::endl;
                    if (gr[chld].deadline == 1 && partitioning[chld] == leaf_proc) {
                        nodes_on_the_same_proc[leaf] += gr[chld].deadline;
                    }
                }
        }


        for (auto [i, j] : nodes_on_the_same_proc) {
            gr[i].deadline = fmin(gr[leaf].deadline, j);
        }
        */
    }

    LOG_INFO << "All deadlines set up";

    TimeDiagram res(sched, sched.proc_num);

    std::vector<ScheduleData::Task> order(sched.task_num);
    std::iota(order.begin(), order.end(), 0);

    std::sort(order.begin(), order.end(),
              [&gr](const ScheduleData::Task &fr, const ScheduleData::Task &scnd) {
                  return gr[fr].deadline < gr[scnd].deadline;
              });

    LOG_INFO << "Got order";

    std::size_t it_counter = 0;
    std::size_t it_counter_max = sched.task_num;
    double last_ratio = it_counter / (double)it_counter_max;

    for (const ScheduleData::Task &cur : order) {
        ScheduleData::Proc chosen_proc = partitioning[cur];

        res.add_task(cur, chosen_proc);

        if (it_counter % 100 == 0) {
            auto ratio = it_counter / (double)it_counter_max;
            if (ratio - last_ratio > 0.01) {
                LOG_DEBUG << "Progress: " << ratio * 100 << "%"
                          << "; time = " << res.get_time();
                last_ratio = ratio;
            }
        }
        ++it_counter;
    }

    LOG_INFO << "Time schedule constructed; time = " << res.get_time();

    return res;
}

} // namespace greedy
} // namespace opts
