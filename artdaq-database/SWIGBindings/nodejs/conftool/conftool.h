#include <string>
#include <utility>

typedef std::pair<bool, std::string> result_t;

result_t load_configuration_ui(std::string const& query_payload);
result_t store_configuration_ui(std::string const& query_payload, std::string const& json_document);
result_t find_global_configurations_ui(std::string const& query_payload);
result_t build_global_configuration_search_filter_ui(std::string const& query_payload);
result_t add_configuration_to_global_configuration_ui(std::string const& query_payload);
result_t create_new_global_configuration_ui(std::string const& query_payload);
result_t find_configuration_versions_ui(std::string const& query_payload);
result_t find_configuration_entities_ui(std::string const& query_payload);
result_t load_globalconfiguration_ui(std::string const& query_payload);
result_t store_globalconfiguration_ui(std::string const& query_payload, std::string const& tarbz2base64);
result_t list_databases(std::string const& query_payload);
result_t read_dbinfo(std::string const& query_payload);
result_t list_collections(std::string const& query_payload);

result_t export_configuration(std::string const& query_payload);  // exportimport
result_t import_configuration(std::string const& query_payload);  // exportimport
result_t export_database(std::string const& query_payload);       // exportimport
result_t import_database(std::string const& query_payload);       // exportimport
result_t export_collection(std::string const& query_payload);     // exportimport
result_t import_collection(std::string const& query_payload);     // exportimport

void enable_trace();
void set_default_locale();