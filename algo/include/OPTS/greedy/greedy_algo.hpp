#ifndef GREEDY_ALGO_HPP_
#define GREEDY_ALGO_HPP_

#include "OPTS/greedy/schedule_data.hpp"
#include "OPTS/greedy/time_diagram.hpp"

namespace opts {
namespace greedy {

/**
 * @brief This is the Greedy_GC1 algorithm.
 * 
 * @param schedule All specialized greedy algorithm input data. 
 * @param conf Greedy algorithm configuration.
 * @retval TimeDiagram Constructed schedule.
 */
TimeDiagram construct_time_schedule(ScheduleData &schedule,
                                     opts::base_config conf);

/**
 * @brief This is the Greedy_EDFBase algorithm.
 * 
 * @param sched All specialized greedy algorithm input data. 
 * @param conf Greedy algorithm configuration.
 * @throws std::runtime_error If ann error occured during inserting a task. 
 * @retval TimeDiagram Constructed schedule.
 */

TimeDiagram greedy_EDFBase_heuristic(ScheduleData &sched, opts::base_config conf);
/**
 * @brief This is the Greedy_EDFFollow algorithm.
 * 
 * @param sched All specialized greedy algorithm input data. 
 * @param conf Greedy algorithm configuration.
 * @throws std::runtime_error If ann error occured during inserting a task. 
 * @retval TimeDiagram Constructed schedule.
 */
TimeDiagram greedy_EDFFollow_heuristic(ScheduleData &sched, opts::base_config conf);

} // namespace greedy
} // namespace opts

#endif
