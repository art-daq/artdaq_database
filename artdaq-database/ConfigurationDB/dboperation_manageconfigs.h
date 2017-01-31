#ifndef _ARTDAQ_DATABASE_CONFIGURATIONDB_MANAGECONFIGS_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONDB_MANAGECONFIGS_H_

#include "artdaq-database/SharedCommon/returned_result.h"

namespace artdaq {
namespace database {
namespace configuration {
class ManageDocumentOperation;

using artdaq::database::result_t;

namespace opts {
using artdaq::database::configuration::ManageDocumentOperation;

result_t find_configurations(ManageDocumentOperation const& /*options*/) noexcept;
result_t assign_configuration(ManageDocumentOperation const& /*options*/) noexcept;
result_t remove_configuration(ManageDocumentOperation const& /*options*/) noexcept;
result_t create_configuration(ManageDocumentOperation const& /*options*/) noexcept;
result_t configuration_composition(ManageDocumentOperation const& /*options*/) noexcept;
}

namespace json {
result_t find_configurations(std::string const& /*task_payload*/) noexcept;
result_t assign_configuration(std::string const& /*task_payload*/) noexcept;
result_t remove_configuration(std::string const& /*task_payload*/) noexcept;
result_t create_configuration(std::string const& /*task_payload*/) noexcept;
result_t configuration_composition(std::string const& /*task_payload*/) noexcept;
}

namespace debug {
void ManageConfigs();
namespace detail {
void ManageConfigs();
}
}

}  // namespace configuration
}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_CONFIGURATIONDB_MANAGECONFIGS_H_ */
