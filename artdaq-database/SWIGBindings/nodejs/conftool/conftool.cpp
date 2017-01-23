#include "artdaq-database/ConfigurationDB/configurationdb.h"
#include "conftool.h"

namespace impl = artdaq::database::configuration::json;

result_t load_configuration_ui(std::string const& query_payload) {
  auto conf = std::string{};
  return impl::read_document(query_payload, conf);
}
result_t store_configuration_ui(std::string const& query_payload, std::string const& json_document) {
  return impl::write_document(query_payload, json_document);
}
result_t find_global_configurations_ui(std::string const& query_payload) {
  return impl::find_configurations(query_payload);
}
result_t build_global_configuration_search_filter_ui(std::string const& query_payload) {
  return impl::configuration_composition(query_payload);
}
result_t add_configuration_to_global_configuration_ui(std::string const& query_payload) {
  return impl::assign_configuration(query_payload);
}
result_t create_new_global_configuration_ui(std::string const& query_payload) {
  return impl::create_configuration(query_payload);
}

result_t find_configuration_versions_ui(std::string const& query_payload) { return impl::find_versions(query_payload); }

result_t find_configuration_entities_ui(std::string const& query_payload) { return impl::find_entities(query_payload); }

result_t load_globalconfiguration_ui(std::string const& query_payload) {
  auto conf = std::string{};
  return impl::read_configuration(query_payload, conf);
}

result_t store_globalconfiguration_ui(std::string const& query_payload, std::string const& json_document) {
  return impl::write_configuration(query_payload, json_document);
}

result_t list_databases(std::string const& query_payload) { return impl::list_databases(query_payload); }

result_t read_dbinfo(std::string const& query_payload) { return impl::read_dbinfo(query_payload); }