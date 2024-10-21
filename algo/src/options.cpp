#include "OPTS/options.hpp"
#include "toml.hpp"

#include <boost/describe/enum_from_string.hpp>

namespace opts {

/**
 * @brief Parse base config.
 *
 * @param data toml tag to start parsing from.
 * @retval base_config Generalised configuration parameters.
 */
base_config parse_base_config(std::string filename) {
    toml::value all_data = toml::parse(filename);
    toml::value &base_section = toml::find(all_data, "general");
    base_config res;
    std::string criteria = toml::find<std::string>(base_section, "criteria");
    boost::describe::enum_from_string(criteria.c_str(), res.criteria);
    res.CR_bound = toml::find<double>(base_section, "CR_bound");
    res.CR2_bound = toml::find<double>(base_section, "CR2_bound");
    std::string input_class = toml::find<std::string>(base_section, "inp_class");
    boost::describe::enum_from_string(input_class.c_str(), res._class);
    return res;
}

} // namespace opts