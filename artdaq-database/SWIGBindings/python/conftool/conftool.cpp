#include "artdaq-database/ConfigurationDB/configurationdb.h"
#include "conftool.h"

namespace impl = artdaq::database::configuration::json;

result_t read_document(std::string const& query_payload) {
  auto conf = std::string{};
  return impl::read_document(query_payload, conf);
}

result_t write_document(std::string const& query_payload, std::string const& json_document) {
  return impl::write_document(query_payload, json_document);
}

result_t mark_document_readonly(std::string const& query_payload) {
  return impl::mark_document_readonly(query_payload);
}

result_t mark_document_deleted(std::string const& query_payload) {
  return impl::mark_document_deleted(query_payload);
}

result_t find_versions(std::string const& query_payload) { return impl::find_versions(query_payload); }

result_t add_version_alias(std::string const& query_payload) { return impl::add_version_alias(query_payload); }

result_t remove_version_alias(std::string const& query_payload) { return impl::remove_version_alias(query_payload); }

result_t find_entities(std::string const& query_payload) { return impl::find_entities(query_payload); }

result_t add_entity(std::string const& query_payload) { return impl::add_entity(query_payload); }

result_t remove_entity(std::string const& query_payload) { return impl::remove_entity(query_payload); }

result_t find_configurations(std::string const& query_payload) { return impl::find_configurations(query_payload); }

result_t configuration_composition(std::string const& query_payload) {
  return impl::configuration_composition(query_payload);
}

result_t create_configuration(std::string const& query_payload) { return impl::create_configuration(query_payload); }

result_t assign_configuration(std::string const& query_payload) { return impl::assign_configuration(query_payload); }

result_t remove_configuration(std::string const& query_payload) { return impl::remove_configuration(query_payload); }

result_t read_configuration(std::string const& query_payload) {
  auto conf = std::string{};
  return impl::read_configuration(query_payload, conf);
}

void enable_trace(){
  return impl::enable_trace();
}
result_t write_configuration(std::string const& query_payload, std::string const& json_document) {
  return impl::write_configuration(query_payload, json_document);
}

result_t add_configuration_alias(std::string const& query_payload) {
  return impl::add_configuration_alias(query_payload);
}

result_t remove_configuration_alias(std::string const& query_payload) {
  return impl::remove_configuration_alias(query_payload);
}

result_t export_configuration(std::string const& query_payload) {
  return impl::export_configuration(query_payload);
}

result_t import_configuration(std::string const& query_payload) {
  return impl::import_configuration(query_payload);
}

result_t list_databases(std::string const& query_payload) { return impl::list_databases(query_payload); }

result_t read_dbinfo(std::string const& query_payload) { return impl::read_dbinfo(query_payload); }

result_t list_collections(std::string const& query_payload){
  return impl::list_collections(query_payload);
}

result_t export_database(std::string const& query_payload) {
  return impl::export_database(query_payload);
}

result_t import_database(std::string const& query_payload) {
  return impl::export_database(query_payload);
}

result_t export_collection(std::string const& query_payload){
    return impl::export_collection(query_payload);
}

result_t import_collection(std::string const& query_payload){
      return impl::import_collection(query_payload);
}
