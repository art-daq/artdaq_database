#ifndef _ARTDAQ_DATABASE_CONFIGURATIONDB_METADATA_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONDB_METADATA_H_

#include <string>
#include <utility>

namespace artdaq {
namespace database {
namespace configuration {
class LoadStoreOperation;

using result_pair_t = std::pair<bool, std::string>;

namespace opts {
using artdaq::database::configuration::LoadStoreOperation;
result_pair_t list_database_names(LoadStoreOperation const& /*options*/, std::string& /*conf*/) noexcept;
result_pair_t read_database_info(LoadStoreOperation const& /*options*/, std::string& /*conf*/) noexcept;
}

namespace json {
result_pair_t list_database_names(std::string const& /*search_filter*/) noexcept;
result_pair_t read_database_info(std::string const& /*search_filter*/) noexcept;
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
