#ifndef _ARTDAQ_DATABASE_CONFIGURATIONDB_MANAGEALIAS_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONDB_MANAGEALIAS_H_

#include <string>
#include <utility>

//#include "artdaq-database/ConfigurationDB/options_operation_managealiases.h"

namespace artdaq {
namespace database {
namespace configuration {
class LoadStoreOperation;

using result_pair_t = std::pair<bool, std::string>;

namespace opts {
using artdaq::database::configuration::LoadStoreOperation;
result_pair_t add_version_alias(LoadStoreOperation const& /*options*/) noexcept;
result_pair_t remove_version_alias(LoadStoreOperation const& /*options*/) noexcept;
result_pair_t find_version_aliases(LoadStoreOperation const& /*options*/) noexcept;
}

namespace json {
result_pair_t add_version_alias(std::string const& /*search_filter*/) noexcept;
result_pair_t remove_version_alias(std::string const& /*search_filter*/) noexcept;
result_pair_t find_version_aliases(std::string const& /*search_filter*/) noexcept;
}
void trace_enable_ManageAliasesOperation();

}  // namespace configuration
}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_CONFIGURATIONDB_VERSIONALIAS_H_ */
