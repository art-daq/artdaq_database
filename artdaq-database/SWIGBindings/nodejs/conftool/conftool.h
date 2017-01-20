#include <string>
#include <utility>

typedef std::pair<bool, std::string> result_t;

result_t read_document_ui(std::string const& query_payload);
result_t write_document_ui(std::string const& query_payload, std::string const& json_document);
result_t find_configurations_ui(std::string const& query_payload);
result_t configuration_composition_ui(std::string const& query_payload);
result_t assign_configuration_ui(std::string const& query_payload);
result_t create_configuration_ui(std::string const& query_payload);
result_t find_versions_ui(std::string const& query_payload);
result_t find_entities_ui(std::string const& query_payload);
result_t read_configuration_ui(std::string const& query_payload);
result_t write_configuration_ui(std::string const& query_payload, std::string const& tarbz2base64);
result_t list_databases(std::string const& query_payload);
result_t read_dbinfo(std::string const& query_payload);