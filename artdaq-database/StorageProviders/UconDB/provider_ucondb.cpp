#include "artdaq-database/StorageProviders/UconDB/provider_ucondb.h"
#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/SharedCommon/common.h"
#include "artdaq-database/StorageProviders/UconDB/ucondb_api.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "provider_ucondb.cpp"

namespace artdaq {
namespace database {

using artdaq::database::docrecord::JSONDocument;
using artdaq::database::ucon::UconDB;

template <>
template <>
std::vector<JSONDocument> StorageProvider<JSONDocument, UconDB>::findConfigurations(JSONDocument const& query_payload) {
  confirm(!query_payload.empty());
  auto returnCollection = std::vector<JSONDocument>();

  TLOG(15) << "StorageProvider::UconDB::findConfigurations() begin";
  TLOG(15) << "StorageProvider::UconDB::findConfigurations() args data=<" << query_payload << ">";

  throw runtime_error("UconDB") << "StorageProvider::UconDB::findConfigurations() is not implemented";

  return returnCollection;
}

template <>
template <>
std::vector<JSONDocument> StorageProvider<JSONDocument, UconDB>::configurationComposition(JSONDocument const& query_payload) {
  confirm(!query_payload.empty());

  auto returnCollection = std::vector<JSONDocument>();

  TLOG(16) << "StorageProvider::UconDB::configurationComposition() begin";
  TLOG(16) << "StorageProvider::UconDB::configurationComposition() args data=<" << query_payload << ">";

  throw runtime_error("UconDB") << "StorageProvider::UconDB::configurationComposition() is not implemented";

  return returnCollection;
}

template <>
template <>
std::vector<JSONDocument> StorageProvider<JSONDocument, UconDB>::findVersions(JSONDocument const& filter) {
  confirm(!filter.empty());

  auto returnCollection = std::vector<JSONDocument>();

  TLOG(15) << "StorageProvider::UconDB::findVersions() begin";
  TLOG(15) << "StorageProvider::UconDB::findVersions() args data=<" << filter << ">";

  throw runtime_error("UconDB") << "StorageProvider::UconDB::findVersions() is not implemented";

  return returnCollection;
}

template <>
template <>
std::vector<JSONDocument> StorageProvider<JSONDocument, UconDB>::findEntities(JSONDocument const& filter) {
  confirm(!filter.empty());
  auto returnCollection = std::vector<JSONDocument>();

  TLOG(19) << "StorageProvider::UconDB::findEntities() begin";
  TLOG(19) << "StorageProvider::UconDB::findEntities() args data=<" << filter << ">";

  throw runtime_error("UconDB") << "StorageProvider::UconDB::findEntities() is not implemented";

  return returnCollection;
}

template <>
template <>
std::vector<JSONDocument> StorageProvider<JSONDocument, UconDB>::listCollections(JSONDocument const& query_payload) {
  confirm(!query_payload.empty());

  auto returnCollection = std::vector<JSONDocument>();
  TLOG(22) << "StorageProvider::UconDB::listCollections() begin";
  TLOG(22) << "StorageProvider::UconDB::listCollections() args data=<" << query_payload << ">";

  throw runtime_error("UconDB") << "StorageProvider::UconDB::listCollections() is not implemented";

  return returnCollection;
}

template <>
template <>
std::vector<JSONDocument> StorageProvider<JSONDocument, UconDB>::addConfiguration(JSONDocument const& query_payload) {
  confirm(!query_payload.empty());
  auto returnCollection = std::vector<JSONDocument>();

  TLOG(15) << "StorageProvider::UconDB::addConfiguration() begin";
  TLOG(15) << "StorageProvider::UconDB::addConfiguration() args data=<" << query_payload << ">";

  throw runtime_error("UconDB") << "StorageProvider::UconDB::addConfiguration() is not implemented";

  return returnCollection;
}

template <>
template <>
std::vector<JSONDocument> StorageProvider<JSONDocument, UconDB>::listDatabases(JSONDocument const& query_payload) {
  confirm(!query_payload.empty());
  auto returnCollection = std::vector<JSONDocument>();

  TLOG(19) << "StorageProvider::UconDB::listDatabases() begin";
  TLOG(19) << "StorageProvider::UconDB::listDatabases() args data=<" << query_payload << ">";

  throw runtime_error("UconDB") << "StorageProvider::UconDB::listDatabases() is not implemented";

  return returnCollection;
}

template <>
template <>
std::vector<JSONDocument> StorageProvider<JSONDocument, UconDB>::databaseMetadata(JSONDocument const& query_payload [[gnu::unused]]) {
  confirm(!query_payload.empty());
  auto returnCollection = std::vector<JSONDocument>();

  throw runtime_error("UconDB") << "StorageProvider::UconDB::databaseMetadata() is not implemented";

  return returnCollection;
}

template <>
template <>
std::vector<JSONDocument> StorageProvider<JSONDocument, UconDB>::searchCollection(JSONDocument const& query_payload) {
  confirm(!query_payload.empty());
  auto returnCollection = std::vector<JSONDocument>();

  TLOG(15) << "StorageProvider::UconDB::searchCollection() begin";
  TLOG(15) << "StorageProvider::UconDB::searchCollection() args data=<" << query_payload << ">";

  throw runtime_error("UconDB") << "StorageProvider::UconDB::searchCollection() is not implemented";

  return returnCollection;
}

namespace ucon {
namespace debug {
void enable() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TLOG(10) << "artdaq::database::ucon trace_enable";
  artdaq::database::ucon::debug::UconDBAPI();
  artdaq::database::ucon::debug::ReadWrite();
}
}  // namespace debug
}  // namespace ucon
}  // namespace database
}  // namespace artdaq
