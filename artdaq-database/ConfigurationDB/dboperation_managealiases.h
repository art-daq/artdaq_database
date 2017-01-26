#ifndef _ARTDAQ_DATABASE_CONFIGURATIONDB_MANAGEALIAS_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONDB_MANAGEALIAS_H_

#include "artdaq-database/SharedCommon/returned_result.h"

namespace artdaq {
namespace database {
namespace configuration {
class ManageDocumentOperation;
class ManageAliasesOperation;

using artdaq::database::result_t;

namespace opts {
using artdaq::database::configuration::ManageDocumentOperation;
using artdaq::database::configuration::ManageAliasesOperation;

result_t add_version_alias(ManageDocumentOperation const& /*options*/) noexcept;
result_t remove_version_alias(ManageDocumentOperation const& /*options*/) noexcept;
result_t find_version_aliases(ManageDocumentOperation const& /*options*/) noexcept;
result_t add_configuration_alias(ManageDocumentOperation const& /*options*/) noexcept;
result_t remove_configuration_alias(ManageDocumentOperation const& /*options*/) noexcept;
}

namespace json {
result_t add_version_alias(std::string const& /*task_payload*/) noexcept;
result_t remove_version_alias(std::string const& /*task_payload*/) noexcept;
result_t find_version_aliases(std::string const& /*task_payload*/) noexcept;
result_t add_configuration_alias(std::string const& /*task_payload*/) noexcept;
result_t remove_configuration_alias(std::string const& /*task_payload*/) noexcept;
}

namespace debug {
void enableManageAliases();
namespace detail {
void enableManageAliases();
}
}

}  // namespace configuration
}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_CONFIGURATIONDB_VERSIONALIAS_H_ */
