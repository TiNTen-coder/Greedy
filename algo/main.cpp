#include <boost/describe/enum_from_string.hpp>
#include <boost/describe/enum_to_string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <boost/timer/timer.hpp>

#include "SCHED/greedy/greedy_algo.hpp"
#include "SCHED/parser.hpp"
#include "SCHED/json_dumper.hpp"
#include "SCHED/logger_config.hpp"
#include "SCHED/options.hpp"

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>

unsigned long mix(unsigned long a, unsigned long b, unsigned long c) {
    a = a - b;
    a = a - c;
    a = a ^ (c >> 13);
    b = b - c;
    b = b - a;
    b = b ^ (a << 8);
    c = c - a;
    c = c - b;
    c = c ^ (b >> 13);
    a = a - b;
    a = a - c;
    a = a ^ (c >> 12);
    b = b - c;
    b = b - a;
    b = b ^ (a << 16);
    c = c - a;
    c = c - b;
    c = c ^ (b >> 5);
    a = a - b;
    a = a - c;
    a = a ^ (c >> 3);
    b = b - c;
    b = b - a;
    b = b ^ (a << 10);
    c = c - a;
    c = c - b;
    c = c ^ (b >> 15);
    return c;
}

int main(int argc, char *argv[]) {
    unsigned long seed = mix(clock(), time(NULL), getpid());
    srand(seed);

    boost::program_options::options_description global("General descriptions");
    global.add_options() //
        ("log,l",
         boost::program_options::value<boost::log::trivial::severity_level>()
             ->default_value(boost::log::trivial::debug),
         "Specify log level \n"
         "0 - trace \n"
         "1 - debug \n"
         "2 - info \n"
         "3 - warning \n"
         "4 - error \n"
         "5 - fatal \n") //
        ("input,i", boost::program_options::value<std::string>()->required(),
         "Input file") //
        ("output,o", boost::program_options::value<std::string>()->required(),
         "Output file") //
        ("conf", boost::program_options::value<std::string>()->required(),
         "Config file path") //
        ("command", boost::program_options::value<std::string>()->required(),
         "algo to execute") //
        ("random", boost::program_options::value<int>()->required(),
         "Fix METIS random flag");
    boost::program_options::positional_options_description pos;
    pos.add("command", 1);
    boost::program_options::variables_map vm;
    boost::program_options::parsed_options parsed =
        boost::program_options::command_line_parser(argc, argv)
            .options(global)
            .positional(pos)
            .run();
    boost::program_options::store(parsed, vm);
    boost::program_options::notify(vm);

    sched::init(vm["log"].as<boost::log::trivial::severity_level>());
    std::string algo = vm["command"].as<std::string>();
    sched::base_config conf =
        sched::parse_base_config(vm["conf"].as<std::string>());
    if (algo == "misf" || algo == "edfbase" || algo == "edffollow" || algo == "edfb_misf"
                    || algo == "edff_misf" || algo == "est" || algo == "eft") {
        LOG_INFO << "Starting";
        boost::timer::cpu_timer timer;
        sched::greedy::ScheduleData schedule = sched::input_schedule_regular(
            vm["input"].as<std::string>(), conf._class);
        sched::greedy::TimeDiagram time_schedule =
            sched::greedy::heuristics(schedule, conf, algo, vm["random"].as<int>());

        auto algo_time = timer.elapsed();

        LOG_INFO << "dumping to " << vm["output"].as<std::string>();

        sched::Output_data out_data = time_schedule.extract_data(conf);
        sched::dump_to_json(vm["output"].as<std::string>(), out_data,
                           (algo_time.system + algo_time.user) / (uint64_t)1e6);
        return 0;
    } else {
        throw std::runtime_error("not a known algo");
    }

    return 0;
}
