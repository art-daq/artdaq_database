#ifndef _ARTDAQ_DATABASE_CONFIGURATIONDB_DISPATCH_FILESYSTEM_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONDB_DISPATCH_FILESYSTEM_H_

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

namespace filesystem {
using artdaq::database::basictypes::JsonData;

namespace cf = artdaq::database::configuration;

void writeDocument(cf::ManageDocumentOperation const& /*options*/, JsonData const& /*insert_payload*/);
JsonData readDocument(cf::ManageDocumentOperation const& /*options*/, JsonData const& /*query_payload*/);

JsonData findConfigurations(cf::ManageConfigsOperation const& /*options*/, JsonData const& /*query_payload*/);
JsonData configurationComposition(cf::ManageConfigsOperation const& /*options*/, JsonData const& /*query_payload*/);

JsonData findVersions(cf::ManageDocumentOperation const& /*options*/, JsonData const& /*query_payload*/);
JsonData addConfiguration(cf::ManageDocumentOperation const& /*options*/, JsonData const& /*query_payload*/);

JsonData findEntities(cf::ManageDocumentOperation const& /*options*/, JsonData const& /*query_payload*/);

JsonData listCollections(cf::ManageDocumentOperation const& /*options*/, JsonData const& /*query_payload*/);

JsonData listDatabases(cf::ManageDocumentOperation const& /*options*/, JsonData const& /*query_payload*/);
JsonData readDbInfo(cf::ManageDocumentOperation const& /*options*/, JsonData const& /*query_payload*/);
}  // namespace filesystem

namespace debug {
void enableDBOperationFileSystem();
}
}  // namespace configuration
}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_CONFIGURATIONDB_DISPATCH_FILESYSTEM_H_ */
