#ifndef _ARTDAQ_DATABASE_CONFIGURATIONDB_MANAGEDOCUMENT_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONDB_MANAGEDOCUMENT_H_

#include "artdaq-database/SharedCommon/returned_result.h"

namespace artdaq {
namespace database {
namespace configuration {
class ManageDocumentOperation;

using artdaq::database::result_t;

namespace opts {
using artdaq::database::configuration::ManageDocumentOperation;

result_t write_document(ManageDocumentOperation const& /*options*/, std::string& /*conf*/) noexcept;
result_t read_document(ManageDocumentOperation const& /*options*/, std::string& /*conf*/) noexcept;
result_t mark_document_readonly(ManageDocumentOperation const& /*options*/) noexcept;
result_t mark_document_deleted(ManageDocumentOperation const& /*options*/) noexcept;
result_t find_versions(ManageDocumentOperation const& /*options*/) noexcept;
result_t find_entities(ManageDocumentOperation const& /*options*/) noexcept;
result_t add_entity(ManageDocumentOperation const& /*options*/) noexcept;
result_t remove_entity(ManageDocumentOperation const& /*options*/) noexcept;
}  // namespace opts

namespace json {
result_t write_document(std::string const& /*task_payload*/, std::string const& /*conf*/) noexcept;
result_t read_document(std::string const& /*task_payload*/, std::string& /*conf*/) noexcept;
result_t mark_document_readonly(std::string const& /*task_payload*/) noexcept;
result_t mark_document_deleted(std::string const& /*task_payload*/) noexcept;
result_t find_versions(std::string const& /*task_payload*/) noexcept;
result_t find_entities(std::string const& /*task_payload*/) noexcept;
result_t add_entity(std::string const& /*task_payload*/) noexcept;
result_t remove_entity(std::string const& /*task_payload*/) noexcept;
}  // namespace json

namespace debug {
void ManageDocuments();
namespace detail {
void ManageDocuments();
}
}  // namespace debug

}  // namespace configuration
}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_CONFIGURATIONDB_MANAGEDOCUMENT_H_ */
