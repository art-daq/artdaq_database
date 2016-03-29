#ifndef _ARTDAQ_DATABASE_CONFIGURATIONDB_LOADSTORE_MONGO_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONDB_LOADSTORE_MONGO_H_

#include "artdaq-database/ConfigurationDB/common.h"

namespace artdaq {
namespace database {
namespace basictypes {
class JsonData;
}  // namespace basictypes
namespace configuration {
namespace options {
class LoadStoreOperation;
class FindConfigsOperation;
}  // namespace options

namespace mongo {
using artdaq::database::basictypes::JsonData;

namespace cfo = artdaq::database::configuration::options;

void store(cfo::LoadStoreOperation const& /*options*/, JsonData const& /*insert_payload*/);
JsonData load(cfo::LoadStoreOperation const& /*options*/, JsonData const& /*search_payload*/);

JsonData findGlobalConfigs(cfo::FindConfigsOperation const& /*options*/, JsonData const& /*search_payload*/);
JsonData buildConfigSearchFilter(cfo::FindConfigsOperation const& /*options*/, JsonData const& /*search_payload*/);

JsonData findConfigVersions(cfo::LoadStoreOperation const& /*options*/, JsonData const& /*search_payload*/);
JsonData addConfigToGlobalConfig(cfo::LoadStoreOperation const& /*options*/, JsonData const& /*search_payload*/);

JsonData findConfigEntities(cfo::LoadStoreOperation const& /*options*/, JsonData const& /*search_payload*/);

}  // namespace mongo
}  // namespace configuration
}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_CONFIGURATIONDB_LOADSTORE_MONGO_H_ */
