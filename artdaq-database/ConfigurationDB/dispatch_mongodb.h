#ifndef _ARTDAQ_DATABASE_CONFIGURATIONDB_DISPATCH_MONGO_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONDB_DISPATCH_MONGO_H_

#include "artdaq-database/ConfigurationDB/common.h"

namespace artdaq {
namespace database {
namespace docrecord {
struct JSONDocument;
}  // namespace basictypes
namespace configuration {
class ManageDocumentOperation;
class ManageAliasesOperation;

namespace mongo {
using artdaq::database::docrecord::JSONDocument;

namespace cf = artdaq::database::configuration;

void writeDocument(cf::ManageDocumentOperation const& /*options*/, JSONDocument const& /*insert_payload*/);
JSONDocument readDocument(cf::ManageDocumentOperation const& /*options*/, JSONDocument const& /*query_payload*/);
std::vector<JSONDocument> readDocuments(cf::ManageDocumentOperation const& /*options*/, JSONDocument const& /*query_payload*/);
JSONDocument markDocumentReadonly(cf::ManageDocumentOperation const& /*options*/, JSONDocument const& /*query_payload*/);
JSONDocument markDocumentDeleted(cf::ManageDocumentOperation const& /*options*/, JSONDocument const& /*query_payload*/);
std::vector<JSONDocument> findVersions(cf::ManageDocumentOperation const& /*options*/, JSONDocument const& /*query_payload*/);
std::vector<JSONDocument> findEntities(cf::ManageDocumentOperation const& /*options*/, JSONDocument const& /*query_payload*/);
JSONDocument addEntity(cf::ManageDocumentOperation const& /*options*/, JSONDocument const& /*query_payload*/);
JSONDocument removeEntity(cf::ManageDocumentOperation const& /*options*/, JSONDocument const& /*query_payload*/);
JSONDocument addVersionAlias(cf::ManageAliasesOperation const& /*options*/, JSONDocument const& /*query_payload*/);
JSONDocument removeVersionAlias(cf::ManageAliasesOperation const& /*options*/, JSONDocument const& /*query_payload*/);
std::vector<JSONDocument> findVersionAliases(cf::ManageAliasesOperation const& /*options*/, JSONDocument const& /*query_payload*/);

JSONDocument addConfigurationAlias(cf::ManageAliasesOperation const& /*options*/, JSONDocument const& /*query_payload*/);
JSONDocument removeConfigurationAlias(cf::ManageAliasesOperation const& /*options*/, JSONDocument const& /*query_payload*/);
std::vector<JSONDocument> findConfigurations(cf::ManageDocumentOperation const& /*options*/, JSONDocument const& /*query_payload*/);
JSONDocument configurationComposition(cf::ManageDocumentOperation const& /*options*/, JSONDocument const& /*query_payload*/);
JSONDocument assignConfiguration(cf::ManageDocumentOperation const& /*options*/, JSONDocument const& /*query_payload*/);
JSONDocument removeConfiguration(cf::ManageDocumentOperation const& /*options*/, JSONDocument const& /*query_payload*/);

std::vector<JSONDocument> listCollections(cf::ManageDocumentOperation const& /*options*/, JSONDocument const& /*query_payload*/);
std::vector<JSONDocument> listDatabases(cf::ManageDocumentOperation const& /*options*/, JSONDocument const& /*query_payload*/);
JSONDocument readDbInfo(cf::ManageDocumentOperation const& /*options*/, JSONDocument const& /*query_payload*/);

}  // namespace mongo
namespace debug {
void MongoDB();
}
}  // namespace configuration
}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_CONFIGURATIONDB_DISPATCH_MONGO_H_ */
