#include <string>
#include <utility>

typedef std::pair<bool, std::string> result_pair_t;

result_pair_t load_configuration_ui(std::string const& search_filter);
result_pair_t store_configuration_ui(std::string const& search_filter, std::string const& json_document);
result_pair_t find_global_configurations_ui(std::string const& search_filter);
result_pair_t build_global_configuration_search_filter_ui(std::string const& search_filter);
result_pair_t add_configuration_to_global_configuration_ui(std::string const& search_filter);
result_pair_t create_new_global_configuration_ui(std::string const& search_filter);
result_pair_t find_configuration_versions_ui(std::string const& search_filter);
result_pair_t find_configuration_entities_ui(std::string const& search_filter);