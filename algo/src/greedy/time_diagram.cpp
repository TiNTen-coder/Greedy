#include "SCHED/greedy/time_diagram.hpp"
#include "SCHED/logger_config.hpp"

#include <algorithm>
#include <iostream>
#include <numeric>

#include <boost/iterator/counting_iterator.hpp>
#include <boost/range/combine.hpp>

namespace sched {
namespace greedy {

TimeDiagram::TimeDiagram(const ScheduleData &schedule, std::size_t proc_num)
    : sched(schedule) {
    proc_array.resize(proc_num);
}

int TimeDiagram::get_time() const {
    BOOST_LOG_NAMED_SCOPE("get_time");
    LOG_TRACE << "calculating time";
    LOG_TRACE << "array size: " << proc_array.size();
    std::vector<std::size_t> proc_times;
    for (const proc_info &a : proc_array) {
        if (!a.empty()) {
            proc_times.push_back(a.back().finish);
        }
    }
    return *std::max_element(proc_times.begin(), proc_times.end());
}

std::size_t TimeDiagram::find_place(greedy::ScheduleData::Task task,
                                     greedy::ScheduleData::Proc proc) {
    BOOST_LOG_NAMED_SCOPE("find_place");
    std::vector<std::size_t> times;
    for (auto it = sched.get_in_edges(task); it.first != it.second;
         ++it.first) {
        auto from = boost::source(*it.first, sched.get_graph());
        int trans_time = sched.tran_times(fast_mapping[from], proc);
        auto &proc_data = proc_array[fast_mapping[from]];
        std::size_t stop_time =
            std::find_if(proc_data.begin(), proc_data.end(),
                         [from](Output_data::PlacedTask &cur) {
                             return cur.task_no == from;
                         })
                ->finish;
        times.push_back(trans_time + stop_time);
    }
    /*
    std::cout << std::endl << std::endl << "какие-то времена в time_diagram" << std::endl;
    for (auto i : times) {
        std::cout << i << " " << sched.get_task_time(fast_mapping[task], task) << std::endl;
    }
    std::cout << std::endl << std::endl;
    */
    std::size_t first_available_dependencies = 0;

    if (!times.empty()) {
        first_available_dependencies =
            *std::max_element(times.begin(), times.end());
    }

    LOG_TRACE << "first_available_dependencies: "
              << first_available_dependencies;

    auto &proc_to_put = proc_array[proc];
    auto first_to_check =
        std::find_if(proc_to_put.begin(), proc_to_put.end(),
                     [first_available_dependencies](auto n) {
                         return n.finish > first_available_dependencies;
                     });
    auto time_to_exec = sched.get_task_time(proc, task);
    if (first_to_check == proc_to_put.end()) {
        LOG_TRACE << "putting task at the end";
        if (!proc_to_put.empty()) {
            LOG_TRACE << "last task at " << proc_to_put.back().finish
                      << " depen: " << first_available_dependencies;
            return std::max(proc_to_put.back().finish,
                            first_available_dependencies);
        } else {
            return first_available_dependencies;
        }
    }

    auto cur = first_to_check;
    for (; cur != std::prev(proc_to_put.end()); ++cur) {
        if (std::next(cur)->start - cur->finish >= time_to_exec) {
            LOG_TRACE << "found hole!";
            return cur->finish;
        }
    }
    LOG_TRACE << "no hole found :(";
    return cur->finish;
}

TimeDiagram::precalc_info
TimeDiagram::test_add_task(greedy::ScheduleData::Task task,
                            greedy::ScheduleData::Proc proc) {
    BOOST_LOG_NAMED_SCOPE("test_add_task");
    auto expected_start = find_place(task, proc);
    add_task_internal(task, proc, expected_start);
    auto res_CR = calculate_CR();
    auto res_CR2 = calculate_CR2();
    remove_task(task);
    return {.starting_place = expected_start,
            .resulting_CR = res_CR,
            .resulting_CR2 = res_CR2,
            .expected_finish =
                expected_start + sched.get_task_time(proc, task)};
}

void TimeDiagram::add_task_internal(greedy::ScheduleData::Task task,
                                     greedy::ScheduleData::Proc proc,
                                     std::size_t starting_place) {
    BOOST_LOG_NAMED_SCOPE("add_task_internal");
    Output_data::PlacedTask placed_task{
        task, starting_place, starting_place + sched.get_task_time(proc, task)};
    LOG_TRACE << "adding: " << task << " from time: " << starting_place
              << " to time: " << placed_task.finish << " on process " << proc;
    auto &proc_to_add_to = proc_array[proc];
    auto place_to_add =
        std::find_if(proc_to_add_to.rbegin(), proc_to_add_to.rend(),
                     [placed_task](Output_data::PlacedTask cur) {
                         return cur.finish <= placed_task.start;
                     });
    LOG_TRACE << "inserting after task " << place_to_add->task_no;
    proc_to_add_to.insert(place_to_add.base(), placed_task);
    fast_mapping[task] = proc;
    for (auto it = sched.get_in_edges(task); it.first != it.second;
         ++it.first) {
        auto from = boost::source(*it.first, sched.get_graph());
        if (fast_mapping[from] != proc) {
            ++amount_of_transitions;
            if (!sched.is_direct_connection(fast_mapping[from], proc))
                ++amount_of_indirect_transitions;
        }
    }
}

void TimeDiagram::add_task(greedy::ScheduleData::Task task,
                            greedy::ScheduleData::Proc proc) {
    BOOST_LOG_NAMED_SCOPE("add_task");
    auto x = find_place(task, proc);
    LOG_TRACE << "adding: " << task << " on time: " << x << " on process "
              << proc;
    add_task_internal(task, proc, x);
}

void TimeDiagram::remove_task(greedy::ScheduleData::Task task) {
    BOOST_LOG_NAMED_SCOPE("remove_task");
    auto &proc = proc_array.at(fast_mapping[task]);
    std::erase_if(proc, [task](const Output_data::PlacedTask &t) {
        return t.task_no == task;
    });
    for (auto it = sched.get_in_edges(task); it.first != it.second;
         ++it.first) {
        auto from = boost::source(*it.first, sched.get_graph());
        if (fast_mapping[from] != fast_mapping[task]) {
            --amount_of_transitions;
            if (!sched.is_direct_connection(fast_mapping[from],
                                            fast_mapping[task]))
                --amount_of_indirect_transitions;
        }
    }
    fast_mapping.erase(task);
}

double TimeDiagram::calculate_CR() const {
    BOOST_LOG_NAMED_SCOPE("calculate_CR");
    LOG_TRACE << amount_of_transitions << " "
              << boost::num_edges(sched.get_graph());
    return amount_of_transitions / (double)boost::num_edges(sched.get_graph());
}

double TimeDiagram::calculate_CR2() const {
    BOOST_LOG_NAMED_SCOPE("calculate_CR2");
    return amount_of_indirect_transitions /
           (double)boost::num_edges(sched.get_graph());
}

void TimeDiagram::print_schedule() const {
    BOOST_LOG_NAMED_SCOPE("print_schedule");
    LOG_TRACE << "printing schedule";
    std::stringstream ss;
    for (auto proc : proc_array) {
        for (auto task : proc) {
            ss << '(' << task.task_no << ")[" << task.start << ','
               << task.finish << "]; ";
        }
        ss << std::endl;
    }
    LOG_TRACE << ss.str();
}

double TimeDiagram::calculate_downtime() const {
    BOOST_LOG_NAMED_SCOPE("calculate_downtime");
    double relative_downtime = 0;
    for (auto proc : proc_array) {
        size_t proc_downtime = 0;
        size_t prev_time = 0;
        size_t task_time = 0;
        if (!proc.empty()) {
            for (auto task : proc) {
                proc_downtime += task.start - prev_time;
                task_time += task.finish - task.start;
                prev_time = task.finish;
            }
            relative_downtime += proc_downtime / (double)task_time;
        }
    }
    return relative_downtime / proc_array.size();
}

std::vector<TimeDiagram::proc_info> &TimeDiagram::get_schedule() {
    return proc_array;
}

Output_data TimeDiagram::extract_data(const sched::base_config &conf) const {
    Output_data res;
    res.CR = calculate_CR();
    res.CR2 = calculate_CR2();
    res.criteria = conf.criteria;
    res.nodes = std::accumulate(
        proc_array.begin(), proc_array.end(), 0,
        [](std::size_t cum, const std::list<Output_data::PlacedTask> &cur) {
            return cum + cur.size();
        });
    res.time = get_time();
    res.proc_array = std::vector<proc_info>(this->proc_array);
    return res;
}

} // namespace greedy
} // namespace sched