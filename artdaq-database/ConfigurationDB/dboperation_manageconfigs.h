#ifndef _ARTDAQ_DATABASE_CONFIGURATIONDB_MANAGECONFIGS_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONDB_MANAGECONFIGS_H_

#include "artdaq-database/SharedCommon/returned_result.h"

namespace artdaq {
namespace database {
namespace configuration {
class ManageConfigsOperation;
class ManageDocumentOperation;

using artdaq::database::result_t;

namespace opts {
using artdaq::database::configuration::ManageConfigsOperation;
using artdaq::database::configuration::ManageDocumentOperation;

result_t find_configurations(ManageConfigsOperation const& /*options*/) noexcept;
result_t assign_configuration(ManageDocumentOperation const& /*options*/) noexcept;
result_t remove_configuration(ManageDocumentOperation const& /*options*/) noexcept;
result_t create_configuration(ManageDocumentOperation const& /*options*/) noexcept;
result_t configuration_composition(ManageConfigsOperation const& /*options*/) noexcept;
result_t find_versions(ManageDocumentOperation const& /*options*/) noexcept;
result_t find_entities(ManageDocumentOperation const& /*options*/) noexcept;
result_t list_collections(ManageDocumentOperation const& /*options*/) noexcept;
}

namespace json {
result_t find_configurations(std::string const& /*task_payload*/) noexcept;
result_t assign_configuration(std::string const& /*task_payload*/) noexcept;
result_t remove_configuration(std::string const& /*task_payload*/) noexcept;
result_t create_configuration(std::string const& /*task_payload*/) noexcept;
result_t configuration_composition(std::string const& /*task_payload*/) noexcept;
result_t find_versions(std::string const& /*task_payload*/) noexcept;
result_t find_entities(std::string const& /*task_payload*/) noexcept;
result_t list_collections(std::string const& /*task_payload*/) noexcept;
}

namespace debug {
void enableCreateConfigsOperation();
void enableFindConfigsOperation();
namespace detail {
void enableCreateConfigsOperation();
void enableFindConfigsOperation();
}
}

}  // namespace configuration
}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_CONFIGURATIONDB_MANAGECONFIGS_H_ */
