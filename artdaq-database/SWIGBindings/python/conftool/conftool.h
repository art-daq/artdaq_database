#include <string>
#include <utility>

typedef std::pair<bool, std::string> result_pair_t;

result_pair_t load_configuration(std::string const& search_filter);
result_pair_t store_configuration(std::string const& search_filter, std::string const& json_document);
result_pair_t find_global_configurations(std::string const& search_filter);
result_pair_t build_global_configuration_search_filter(std::string const& search_filter);
result_pair_t add_configuration_to_global_configuration(std::string const& search_filter);
result_pair_t create_new_global_configuration(std::string const& search_filter);
result_pair_t find_configuration_versions(std::string const& search_filter);
result_pair_t find_configuration_entities(std::string const& search_filter);
result_pair_t load_globalconfiguration(std::string const& search_filter);
result_pair_t store_globalconfiguration(std::string const& search_filter, std::string const& tarbz2base64);
result_pair_t list_database_names(std::string const& search_filter);
result_pair_t read_database_info(std::string const& search_filter);