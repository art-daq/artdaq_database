#ifndef _ARTDAQ_DATABASE_CONFIGURATIONDB_CREATECONFIGS_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONDB_CREATECONFIGS_H_

#include "artdaq-database/ConfigurationDB/dboperations_common.h"

namespace artdaq {
namespace database {
namespace configuration {
namespace options {
class FindConfigsOperation;
}  // namespace options

// both functions return converted conf file
result_pair_t add_configuration_to_global_configuration(options::FindConfigsOperation const& /*options*/) noexcept;
result_pair_t create_new_global_configuration(options::FindConfigsOperation const& /*options*/) noexcept;
result_pair_t find_configuration_versions(options::FindConfigsOperation const& /*options*/) noexcept;

namespace guiexports {
result_pair_t add_configuration_to_global_configuration(std::string const& /*search_filter*/) noexcept;
result_pair_t create_new_global_configuration(std::string const& /*search_filter*/) noexcept;
result_pair_t find_configuration_versions(std::string const& /*search_filter*/) noexcept;
}

void trace_enable_CreateConfigsOperation();
void trace_enable_CreateConfigsOperationDetail();
void trace_enable_DBOperationMongo();
void trace_enable_DBOperationFileSystem();
}  // namespace configuration
}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_CONFIGURATIONDB_CREATECONFIGS_H_ */
