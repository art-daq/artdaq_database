#include <string>
#include <utility>

typedef std::pair<bool, std::string> result_t;

result_t read_document(std::string const& query_payload);
result_t write_document(std::string const& query_payload, std::string const& json_document);
result_t find_configurations(std::string const& query_payload);
result_t configuration_composition(std::string const& query_payload);
result_t assign_configuration(std::string const& query_payload);
result_t create_configuration(std::string const& query_payload);
result_t find_versions(std::string const& query_payload);
result_t find_entities(std::string const& query_payload);
result_t read_configuration(std::string const& query_payload);
result_t write_configuration(std::string const& query_payload, std::string const& tarbz2base64);
result_t list_databases(std::string const& query_payload);
result_t read_dbinfo(std::string const& query_payload);