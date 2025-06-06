#ifndef GREEDY_ALGO_HPP_
#define GREEDY_ALGO_HPP_

#include "SCHED/greedy/schedule_data.hpp"
#include "SCHED/greedy/time_diagram.hpp"

namespace sched {
namespace greedy {

/**
 * @brief This is the Greedy_GC1 algorithm.
 * 
 * @param schedule All specialized greedy algorithm input data. 
 * @param conf Greedy algorithm configuration.
 * @retval TimeDiagram Constructed schedule.
 */
TimeDiagram construct_time_schedule(ScheduleData &schedule,
                                     sched::base_config conf);

/**
 * @brief This is the Greedy_EDFBase algorithm.
 * 
 * @param sched All specialized greedy algorithm input data. 
 * @param conf Greedy algorithm configuration.
 * @throws std::runtime_error If ann error occured during inserting a task. 
 * @retval TimeDiagram Constructed schedule.
 */

TimeDiagram heuristics(ScheduleData &sched, sched::base_config conf, std::string flag, int random_flag);
/**
 * @brief This is the Greedy_EDFFollow algorithm.
 * 
 * @param sched All specialized greedy algorithm input data. 
 * @param conf Greedy algorithm configuration.
 * @throws std::runtime_error If ann error occured during inserting a task. 
 * @retval TimeDiagram Constructed schedule.
 */

} // namespace greedy
} // namespace sched

#endif
