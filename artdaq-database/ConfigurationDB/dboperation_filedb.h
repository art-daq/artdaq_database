#ifndef _ARTDAQ_DATABASE_CONFIGURATIONDB_LOADSTORE_FILESYSTEM_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONDB_LOADSTORE_FILESYSTEM_H_

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

namespace filesystem {
using artdaq::database::basictypes::JsonData;

namespace cfo = artdaq::database::configuration::options;

void store(cfo::LoadStoreOperation const& /*options*/, JsonData const& /*insert_payload*/);
JsonData load(cfo::LoadStoreOperation const& /*options*/, JsonData const& /*search_payload*/);

std::vector<JsonData> findGlobalConfigs(cfo::FindConfigsOperation const& /*options*/,
                                        JsonData const& /*search_payload*/);
std::vector<JsonData> buildConfigSearchFilter(cfo::FindConfigsOperation const& /*options*/,
                                              JsonData const& /*search_payload*/);

}  // namespace filesystem
}  // namespace configuration
}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_CONFIGURATIONDB_LOADSTORE_FILESYSTEM_H_ */
