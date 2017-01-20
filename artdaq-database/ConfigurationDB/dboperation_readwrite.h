#ifndef _ARTDAQ_DATABASE_CONFIGURATIONDB_READWRITE_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONDB_READWRITE_H_

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
}

namespace json {
result_t write_document(std::string const& /*task_payload*/, std::string const& /*conf*/) noexcept;
result_t read_document(std::string const& /*task_payload*/, std::string& /*conf*/) noexcept;
}

namespace debug {
void enableManageDocumentOperation();
namespace detail {
void enableManageDocumentOperation();
}
}

}  // namespace configuration
}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_CONFIGURATIONDB_READWRITE_H_ */
