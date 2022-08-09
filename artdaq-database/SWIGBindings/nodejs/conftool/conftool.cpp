#include "conftool.h"
#include "artdaq-database/ConfigurationDB/configurationdb.h"

namespace impl = artdaq::database::configuration::json;

result_t load_configuration_ui(std::string const& query_payload) {
  set_default_locale();
  auto conf = std::string{};

  return impl::read_document(query_payload, conf);
}

result_t store_configuration_ui(std::string const& query_payload, std::string const& json_document) {
  set_default_locale();

  return impl::write_document(query_payload, json_document);
}

result_t find_global_configurations_ui(std::string const& query_payload) {
  set_default_locale();

  return impl::find_configurations(query_payload);
}
result_t build_global_configuration_search_filter_ui(std::string const& query_payload) {
  set_default_locale();

  return impl::configuration_composition(query_payload);
}
result_t add_configuration_to_global_configuration_ui(std::string const& query_payload) {
  set_default_locale();

  return impl::assign_configuration(query_payload);
}
result_t create_new_global_configuration_ui(std::string const& query_payload) {
  set_default_locale();

  return impl::create_configuration(query_payload);
}

result_t find_configuration_versions_ui(std::string const& query_payload) {
  set_default_locale();

  return impl::find_versions(query_payload);
}

result_t find_configuration_entities_ui(std::string const& query_payload) {
  set_default_locale();

  return impl::find_entities(query_payload);
}

result_t load_globalconfiguration_ui(std::string const& query_payload) {
  set_default_locale();
  auto conf = std::string{};

  return impl::read_configuration(query_payload, conf);
}

result_t store_globalconfiguration_ui(std::string const& query_payload, std::string const& json_document) {
  set_default_locale();

  return impl::write_configuration(query_payload, json_document);
}

result_t list_databases(std::string const& query_payload) {
  set_default_locale();

  return impl::list_databases(query_payload);
}

result_t read_dbinfo(std::string const& query_payload) {
  set_default_locale();

  return impl::read_dbinfo(query_payload);
}

result_t list_collections(std::string const& query_payload) {
  set_default_locale();

  return impl::list_collections(query_payload);
}

result_t export_configuration(std::string const& query_payload) {
  set_default_locale();

  return impl::export_configuration(query_payload);
}

result_t import_configuration(std::string const& query_payload) {
  set_default_locale();

  return impl::import_configuration(query_payload);
}

result_t export_database(std::string const& query_payload) {
  set_default_locale();

  return impl::export_database(query_payload);
}

result_t import_database(std::string const& query_payload) {
  set_default_locale();

  return impl::export_database(query_payload);
}

result_t export_collection(std::string const& query_payload) {
  set_default_locale();

  return impl::export_collection(query_payload);
}

result_t import_collection(std::string const& query_payload) {
  set_default_locale();

  return impl::import_collection(query_payload);
}

result_t search_collection(std::string const& query_payload) {
  set_default_locale();

  return impl::search_collection(query_payload);
}

void enable_trace() { return impl::enable_trace(); }

void set_default_locale() { artdaq::database::set_default_locale(); }
