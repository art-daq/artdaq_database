#ifndef _ARTDAQ_DATABASE_CONFIGURATIONDB_DISPATCH_UCOND_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONDB_DISPATCH_UCOND_H_

#include "artdaq-database/ConfigurationDB/common.h"

namespace artdaq {
namespace database {
namespace basictypes {
class JsonData;
}  // namespace basictypes
namespace configuration {
class ManageDocumentOperation;
class ManageConfigsOperation;
class ManageAliasesOperation;

namespace ucon {
using artdaq::database::basictypes::JsonData;

namespace cf = artdaq::database::configuration;

void store(cf::ManageDocumentOperation const& /*options*/, JsonData const& /*insert_payload*/);
JsonData load(cf::ManageDocumentOperation const& /*options*/, JsonData const& /*query_payload*/);

JsonData findConfigurations(cf::ManageConfigsOperation const& /*options*/, JsonData const& /*query_payload*/);
JsonData configurationComposition(cf::ManageConfigsOperation const& /*options*/, JsonData const& /*query_payload*/);

JsonData findVersions(cf::ManageDocumentOperation const& /*options*/, JsonData const& /*query_payload*/);
JsonData addConfiguration(cf::ManageDocumentOperation const& /*options*/, JsonData const& /*query_payload*/);

JsonData findEntities(cf::ManageDocumentOperation const& /*options*/, JsonData const& /*query_payload*/);

JsonData listCollections(cf::ManageDocumentOperation const& /*options*/, JsonData const& /*query_payload*/);

JsonData listDatabases(cf::ManageDocumentOperation const& /*options*/, JsonData const& /*query_payload*/);
JsonData readDbInfo(cf::ManageDocumentOperation const& /*options*/, JsonData const& /*query_payload*/);

}  // namespace mongo
namespace debug {
void enableDBOperationUcond();
}
}  // namespace configuration
}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_CONFIGURATIONDB_DISPATCH_UCOND_H_ */
