#ifndef _ARTDAQ_DATABASE_CONFIGURATIONDB_GLOBALCONFIGLOADSTORE_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONDB_GLOBALCONFIGLOADSTORE_H_

#include <string>
#include <utility>

namespace artdaq {
namespace database {
namespace configuration {
class LoadStoreOperation;

using result_pair_t = std::pair<bool, std::string>;

namespace json {
result_pair_t store_globalconfiguration(std::string const& /*search_filter*/, std::string const& /*conf*/) noexcept;
result_pair_t load_globalconfiguration(std::string const& /*search_filter*/, std::string& /*conf*/) noexcept;
}

namespace debug {
void enableGlobalConfLoadStoreOperation();
namespace detail {
void enableGlobalConfLoadStoreOperation();
}
}

}  // namespace configuration
}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_CONFIGURATIONDB_GLOBALCONFIGLOADSTORE_H_ */
