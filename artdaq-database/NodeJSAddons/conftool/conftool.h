#include <string>
#include <utility>

typedef std::pair<bool, std::string> result_pair_t;

result_pair_t load_configuration(std::string const& search_filter);
result_pair_t store_configuration(std::string const& search_filter, std::string const& json_document);
result_pair_t find_global_configurations(std::string const& search_filter);
result_pair_t build_global_configuration_search_filter(std::string const& search_filter);