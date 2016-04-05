#ifndef _ARTDAQ_DATABASE_CONFIGURATIONDB_LOADSTORE_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONDB_LOADSTORE_H_

#include <string>
#include <utility>

//#include "artdaq-database/ConfigurationDB/options_operation_loadstore.h"

namespace artdaq {
namespace database {
namespace configuration {
class LoadStoreOperation;

using result_pair_t = std::pair<bool, std::string>;

namespace opts {
using artdaq::database::configuration::LoadStoreOperation;
result_pair_t store_configuration(LoadStoreOperation const& /*options*/, std::string& /*conf*/) noexcept;
result_pair_t load_configuration(LoadStoreOperation const& /*options*/, std::string& /*conf*/) noexcept;
}

namespace json {
result_pair_t store_configuration(std::string const& /*search_filter*/, std::string const& /*conf*/) noexcept;
result_pair_t load_configuration(std::string const& /*search_filter*/, std::string& /*conf*/) noexcept;
}

void trace_enable_LoadStoreOperation();

}  // namespace configuration
}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_CONFIGURATIONDB_LOADSTORE_H_ */
