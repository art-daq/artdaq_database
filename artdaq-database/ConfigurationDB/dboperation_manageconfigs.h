#ifndef _ARTDAQ_DATABASE_CONFIGURATIONDB_MANAGECONFIGS_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONDB_MANAGECONFIGS_H_

#include <string>
#include <utility>

namespace artdaq {
namespace database {
namespace configuration {
class ManageConfigsOperation;
class LoadStoreOperation;

using result_pair_t = std::pair<bool, std::string>;

namespace opts {
using artdaq::database::configuration::ManageConfigsOperation;
using artdaq::database::configuration::LoadStoreOperation;
result_pair_t find_global_configurations(ManageConfigsOperation const& /*options*/) noexcept;
result_pair_t build_global_configuration_search_filter(ManageConfigsOperation const& /*options*/) noexcept;
result_pair_t add_configuration_to_global_configuration(LoadStoreOperation const& /*options*/) noexcept;
result_pair_t create_new_global_configuration(LoadStoreOperation const& /*options*/) noexcept;
result_pair_t find_configuration_versions(LoadStoreOperation const& /*options*/) noexcept;
result_pair_t find_configuration_entities(LoadStoreOperation const& /*options*/) noexcept;
}

namespace json {
result_pair_t find_global_configurations(std::string const& /*search_filter*/) noexcept;
result_pair_t build_global_configuration_search_filter(std::string const& /*search_filter*/) noexcept;
result_pair_t add_configuration_to_global_configuration(std::string const& /*search_filter*/) noexcept;
result_pair_t create_new_global_configuration(std::string const& /*search_filter*/) noexcept;
result_pair_t find_configuration_versions(std::string const& /*search_filter*/) noexcept;
result_pair_t find_configuration_entities(std::string const& /*search_filter*/) noexcept;
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
