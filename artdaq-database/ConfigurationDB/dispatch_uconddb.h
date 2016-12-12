#ifndef _ARTDAQ_DATABASE_CONFIGURATIONDB_DISPATCH_UCOND_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONDB_DISPATCH_UCOND_H_

#include "artdaq-database/ConfigurationDB/common.h"

namespace artdaq {
namespace database {
namespace basictypes {
class JsonData;
}  // namespace basictypes
namespace configuration {
class LoadStoreOperation;
class ManageConfigsOperation;
class ManageAliasesOperation;

namespace ucond {
using artdaq::database::basictypes::JsonData;

namespace cf = artdaq::database::configuration;

void store(cf::LoadStoreOperation const& /*options*/, JsonData const& /*insert_payload*/);
JsonData load(cf::LoadStoreOperation const& /*options*/, JsonData const& /*search_payload*/);

JsonData findGlobalConfigs(cf::ManageConfigsOperation const& /*options*/, JsonData const& /*search_payload*/);
JsonData buildConfigSearchFilter(cf::ManageConfigsOperation const& /*options*/, JsonData const& /*search_payload*/);

JsonData findConfigVersions(cf::LoadStoreOperation const& /*options*/, JsonData const& /*search_payload*/);
JsonData addConfigToGlobalConfig(cf::LoadStoreOperation const& /*options*/, JsonData const& /*search_payload*/);

JsonData findConfigEntities(cf::LoadStoreOperation const& /*options*/, JsonData const& /*search_payload*/);

JsonData listCollectionNames(cf::LoadStoreOperation const& /*options*/, JsonData const& /*search_payload*/);

JsonData listDatabaseNames(cf::LoadStoreOperation const& /*options*/, JsonData const& /*search_payload*/);
JsonData readDatabaseInfo(cf::LoadStoreOperation const& /*options*/, JsonData const& /*search_payload*/);

}  // namespace mongo
namespace debug {
void enableDBOperationUcond();
}
}  // namespace configuration
}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_CONFIGURATIONDB_DISPATCH_UCOND_H_ */
