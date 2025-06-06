#ifndef TIME_SCHEDULE_HPP
#define TIME_SCHEDULE_HPP

#include "SCHED/greedy/schedule_data.hpp"
#include "SCHED/options.hpp"
#include "SCHED/output_class.hpp"

#include <boost/describe/enum.hpp>
#include <boost/numeric/ublas/matrix.hpp>

#include <map>

namespace sched {
namespace greedy {

/**
 * @brief Class representing a time diagram of a schedule
 */
class TimeDiagram {
  public:
    /**
     * @brief Type used to represent one processor
     */
    using proc_info = std::list<Output_data::PlacedTask>;

    /**
     * @brief Internal array that holds the schedule itself
     */
    std::vector<proc_info> proc_array;

    /**
     * @brief internal mapping that is used for optimization to get processor
     * for the task by task id
     */
    std::unordered_map<ScheduleData::Task, ScheduleData::Proc> fast_mapping;

    /**
     * @deprecated
     * @brief Return the underlying processor array
     *
     * @retval std::vector<TimeDiagram::proc_info>& Internal schedule object
     */
    std::vector<TimeDiagram::proc_info> &get_schedule();

  private:
    /**
     * @brief Structure used to represent internal type for trial scheduling
     */
    struct precalc_info {
        /** time the task may start on the processor */
        std::size_t starting_place;
        /** Resulting CR that the schedule may have if the task was placed */
        double resulting_CR;
        /** Resulting CR2 that the schedule may have if the task was placed */
        double resulting_CR2;
        /** Time that the task is expected to finish on the processor */
        std::size_t expected_finish;
    };

    /**
     * @brief internal counter used to calculate CR
     */
    std::size_t amount_of_transitions = 0;

    /**
     * @brief internal counter used to calculate CR2
     */
    std::size_t amount_of_indirect_transitions = 0;

    /**
     * @brief reference to the schedule as a class of input data and parameters
     */
    const ScheduleData &sched;

    /**
     * @brief Calculate the time to which the task can be added.
     *
     * This is the gap filling algorithm. First, for each parent task this
     * function calculates first available space, taking in account the finish
     * of a parent task and inter-processor communication time. After taking the
     * maximum of such values the function considers all gaps after the
     * maximum. If the task fits in a gap the task is placed in a gap.
     *
     * @param task Task to be added to the schedule
     * @param proc Processor for the task
     * @return std::size_t Time the task can start on a processor
     */
    std::size_t find_place(ScheduleData::Task task, ScheduleData::Proc proc);

    /**
     * @brief Test if a task can be added to the schedule.
     *
     * This procedure adds the task to the schedule (involving gap filling
     * algorithm), calculates various metrics and then removes the task from the
     * schedule.
     *
     * @param task Task for obtaining trial scheduling data.
     * @param proc Processor for obtaining trial scheduling data.
     * @retval precalc_info All needed trial scheduling data to place task.
     */
    precalc_info test_add_task(ScheduleData::Task task, ScheduleData::Proc proc);

    /**
     * @brief Remove task from schedule.
     *
     * Take into account `unordered_map` mapping and
     * correctness of CR criteria
     *
     * @param task Task to be removed from the schedule
     */
    void remove_task(ScheduleData::Task task);

    /**
     * @brief Place the task on a processor on a predetermined
     * point of time
     *
     * @param task Task to add to the schedule
     * @param proc Processor for the task
     * @param starting_place Time that the task starts on the processor
     */
    void add_task_internal(ScheduleData::Task task, ScheduleData::Proc proc,
                           std::size_t starting_place);

  public:
    /**
     * @param schedule Input data for schedule construction.
     * @param proc_num Number of processors for the schedule.
     */
    TimeDiagram(const ScheduleData &schedule, std::size_t proc_num);

    TimeDiagram(const TimeDiagram &other) = default;

    /**
     * @brief Calculate schedule duration.
     *
     * @retval int Duration of the schedule.
     */
    int get_time() const;

    /**
     * @param task Task to add to schedule
     * @param proc Processor to add task on
     */
    void add_task(ScheduleData::Task task, ScheduleData::Proc proc);

    /**
     * @brief Calculate `CR` of current schedule
     *
     * @retval double `CR`
     */
    double calculate_CR() const;

    /**
     * @brief Calculate `CR2` of current schedule
     *
     * @retval double `CR2`
     */
    double calculate_CR2() const;

    /**
     * @brief Print schedule to trace level logging
     *
     */
    void print_schedule() const;

    /**
     * @deprecated
     * @brief Calculate downtime metric
     *
     * This metric is equal to 1 minus the sum of all task times divided by all
     * processor times (gaps included)
     *
     * @retval double Downtime metric
     */
    double calculate_downtime() const;

    /**
     * @brief Extract all data into generalised output data to further dump into
     * a json.
     *
     * @param conf Greedy algorithm configuration.
     * @return Output_data Generalised output data.
     */
    Output_data extract_data(const sched::base_config &conf) const;
};

} // namespace greedy
} // namespace sched

#endif