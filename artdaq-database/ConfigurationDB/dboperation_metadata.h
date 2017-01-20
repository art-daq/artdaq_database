#ifndef _ARTDAQ_DATABASE_CONFIGURATIONDB_METADATA_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONDB_METADATA_H_

#include "artdaq-database/SharedCommon/returned_result.h"

namespace artdaq {
namespace database {
namespace configuration {
class ManageDocumentOperation;

using artdaq::database::result_t;

namespace opts {
using artdaq::database::configuration::ManageDocumentOperation;

result_t list_databases(ManageDocumentOperation const& /*options*/, std::string& /*conf*/) noexcept;
result_t read_dbinfo(ManageDocumentOperation const& /*options*/, std::string& /*conf*/) noexcept;
}

namespace json {
result_t list_databases(std::string const& /*task_payload*/) noexcept;
result_t read_dbinfo(std::string const& /*task_payload*/) noexcept;
}

namespace debug {
void enableMetadataOperation();
namespace detail {
void enableMetadataOperation();
}
}

}  // namespace configuration
}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_CONFIGURATIONDB_METADATA_H_ */
