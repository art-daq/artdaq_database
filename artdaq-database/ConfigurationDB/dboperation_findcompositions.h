#ifndef _ARTDAQ_DATABASE_CONFIGURATIONDB_FINDCOMPOSITIONS_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONDB_FINDCOMPOSITIONS_H_

#include "artdaq-database/SharedCommon/returned_result.h"

namespace artdaq {
namespace database {
namespace configuration {
class ManageDocumentOperation;

using artdaq::database::result_t;

namespace opts {
using artdaq::database::configuration::ManageDocumentOperation;
result_t find_compositions_containing(ManageDocumentOperation const& options, std::string& results) noexcept;
}  // namespace opts

namespace json {
result_t find_compositions_containing(std::string const& task_payload) noexcept;
}  // namespace json

namespace debug {
void FindCompositions();
namespace detail {
void FindCompositions();
}
}  // namespace debug

}  // namespace configuration
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_CONFIGURATIONDB_FINDCOMPOSITIONS_H_ */
