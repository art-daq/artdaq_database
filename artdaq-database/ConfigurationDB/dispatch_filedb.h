#ifndef _ARTDAQ_DATABASE_CONFIGURATIONDB_DISPATCH_FILESYSTEM_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONDB_DISPATCH_FILESYSTEM_H_

#include "artdaq-database/ConfigurationDB/common.h"

namespace artdaq {
namespace database {
namespace basictypes {
class JsonData;
}  // namespace basictypes
namespace configuration {
namespace options {
class LoadStoreOperation;
class ManageConfigsOperation;
class ManageAliasesOperation;
}  // namespace options

namespace filesystem {
using artdaq::database::basictypes::JsonData;

namespace cfo = artdaq::database::configuration::options;

void store(cfo::LoadStoreOperation const& /*options*/, JsonData const& /*insert_payload*/);
JsonData load(cfo::LoadStoreOperation const& /*options*/, JsonData const& /*search_payload*/);

JsonData findGlobalConfigs(cfo::ManageConfigsOperation const& /*options*/, JsonData const& /*search_payload*/);
JsonData buildConfigSearchFilter(cfo::ManageConfigsOperation const& /*options*/, JsonData const& /*search_payload*/);

JsonData findConfigVersions(cfo::LoadStoreOperation const& /*options*/, JsonData const& /*search_payload*/);
JsonData addConfigToGlobalConfig(cfo::LoadStoreOperation const& /*options*/, JsonData const& /*search_payload*/);

JsonData findConfigEntities(cfo::LoadStoreOperation const& /*options*/, JsonData const& /*search_payload*/);

}  // namespace filesystem

void trace_enable_DBOperationFileSystem();

}  // namespace configuration
}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_CONFIGURATIONDB_DISPATCH_FILESYSTEM_H_ */
