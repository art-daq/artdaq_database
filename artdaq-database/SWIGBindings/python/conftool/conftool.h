#include <string>
#include <utility>

typedef std::pair<bool, std::string> result_t;

void set_default_locale();

result_t read_document(std::string const& query_payload);  // manage doc

result_t write_document(std::string const& query_payload, std::string const& json_document);  // managedoc
result_t mark_document_readonly(std::string const& query_payload);                            // manage doc
result_t mark_document_deleted(std::string const& query_payload);                             // manage doc
result_t find_versions(std::string const& query_payload);                                     // manage doc
result_t find_entities(std::string const& query_payload);                                     // manage doc
result_t add_entity(std::string const& query_payload);                                        // manage doc
result_t remove_entity(std::string const& query_payload);                                     // manage doc

result_t add_version_alias(std::string const& query_payload);
result_t remove_version_alias(std::string const& query_payload);

result_t find_version_aliases(std::string const& query_payload);
/*
result_t add_configuration_alias(std::string const& query_payload);     // manage configs
result_t remove_configuration_alias(std::string const& query_payload);  // manage configs
*/

result_t find_configurations(std::string const& query_payload);        // manage configs
result_t configuration_composition(std::string const& query_payload);  // manage configs
result_t create_configuration(std::string const& query_payload);       // manage configs
result_t assign_configuration(std::string const& query_payload);       // manage configs
result_t remove_configuration(std::string const& query_payload);       // manage configs

result_t read_configuration(std::string const& query_payload);                                     // exportimport
result_t write_configuration(std::string const& query_payload, std::string const& json_document);  // exportimport

result_t export_configuration(std::string const& query_payload);  // exportimport
result_t import_configuration(std::string const& query_payload);  // exportimport
result_t export_database(std::string const& query_payload);       // exportimport
result_t import_database(std::string const& query_payload);       // exportimport
result_t export_collection(std::string const& query_payload);     // exportimport
result_t import_collection(std::string const& query_payload);     // exportimport

result_t search_collection(std::string const& query_payload);  // search collection

result_t list_databases(std::string const& query_payload);    // metadata
result_t read_dbinfo(std::string const& query_payload);       // metadata
result_t list_collections(std::string const& query_payload);  // metadata

#ifdef ADFHICLCPP
result_t fhicl_to_json(std::string const& fcl, std::string const& filename);  // utility function
result_t json_to_fhicl(std::string const& jsn, std::string& filename);        // utility function
#endif
void enable_trace();
