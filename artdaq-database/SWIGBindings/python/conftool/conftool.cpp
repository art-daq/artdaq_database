#include "artdaq-database/ConfigurationDB/configurationdb.h"
#include "conftool.h"

namespace impl = artdaq::database::configuration::json;

result_pair_t load_configuration(std::string const& search_filter) {
  auto conf = std::string{};
  return impl::load_configuration(search_filter, conf);
}
result_pair_t store_configuration(std::string const& search_filter, std::string const& json_document) {
  return impl::store_configuration(search_filter, json_document);
}
result_pair_t find_global_configurations(std::string const& search_filter) { return impl::find_global_configurations(search_filter); }
result_pair_t build_global_configuration_search_filter(std::string const& search_filter) {
  return impl::build_global_configuration_search_filter(search_filter);
}
result_pair_t add_configuration_to_global_configuration(std::string const& search_filter) {
  return impl::add_configuration_to_global_configuration(search_filter);
}
result_pair_t create_new_global_configuration(std::string const& search_filter) { return impl::create_new_global_configuration(search_filter); }
result_pair_t find_configuration_versions(std::string const& search_filter) { return impl::find_configuration_versions(search_filter); }
result_pair_t find_configuration_entities(std::string const& search_filter) { return impl::find_configuration_entities(search_filter); }
result_pair_t load_globalconfiguration(std::string const& search_filter) {
  auto conf = std::string{};
  return impl::load_globalconfiguration(search_filter, conf);
}
result_pair_t store_globalconfiguration(std::string const& search_filter, std::string const& json_document) {
  return impl::store_globalconfiguration(search_filter, json_document);
}
result_pair_t list_database_names(std::string const& search_filter) { return impl::list_database_names(search_filter); }
result_pair_t read_database_info(std::string const& search_filter) { return impl::read_database_info(search_filter); }