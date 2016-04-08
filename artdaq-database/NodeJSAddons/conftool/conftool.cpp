#include "conftool.h"
#include "artdaq-database/ConfigurationDB/configurationdb.h"
result_pair_t load_configuration_ui(std::string const& search_filter) {return load_configuration(search_filter);}
result_pair_t store_configuration_ui(std::string const& search_filter, std::string const& json_document) {return store_configuration(search_filter,json_document);}
result_pair_t find_global_configurations_ui(std::string const& search_filter) {return  find_global_configurations(search_filter); }
result_pair_t build_global_configuration_search_filter_ui(std::string const& search_filter) {return build_global_configuration_search_filter(search_filter);}
result_pair_t add_configuration_to_global_configuration_ui(std::string const& search_filter) {return add_configuration_to_global_configuration(search_filter);}
result_pair_t create_new_global_configuration_ui(std::string const& search_filter) {return create_new_global_configuration(search_filter);}
result_pair_t find_configuration_versions_ui(std::string const& search_filter) {return find_configuration_versions(search_filter);}
result_pair_t find_configuration_entities_ui(std::string const& search_filter) {return find_configuration_entities(search_filter);} 