#ifndef _ARTDAQ_DATABASE_CONFIGURATIONDB_EXPORTIMPORT_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONDB_EXPORTIMPORT_H_

#include "artdaq-database/SharedCommon/returned_result.h"

namespace artdaq {
namespace database {
namespace configuration {
class ManageDocumentOperation;
class ManageConfigsOperation;

using artdaq::database::result_t;

namespace opts {
result_t export_configuration(ManageConfigsOperation const& /*options*/) noexcept;
result_t import_configuration(ManageConfigsOperation const& /*options*/) noexcept;
}

namespace json {
result_t write_configuration(std::string const& /*task_payload*/, std::string const& /*conf*/) noexcept;
result_t read_configuration(std::string const& /*task_payload*/, std::string& /*conf*/) noexcept;
result_t export_configuration(std::string const& /*task_payload*/) noexcept;
result_t import_configuration(std::string const& /*task_payload*/) noexcept;
}

namespace debug {
void enableConfigurationManageDocumentOperation();
namespace detail {
void enableConfigurationManageDocumentOperation();
}
}

}  // namespace configuration
}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_CONFIGURATIONDB_EXPORTIMPORT_H_ */
