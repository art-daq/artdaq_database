#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/SharedCommon/common.h"
#include "artdaq-database/StorageProviders/UconDB/provider_ucondb.h"
#include "artdaq-database/StorageProviders/UconDB/ucondb_api.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "PRVDR:UconDB_C"

namespace db = artdaq::database;
namespace dbfsl = db::ucon::literal;
namespace jsonliteral = artdaq::database::dataformats::literal;

namespace artdaq {
namespace database {

using artdaq::database::basictypes::JsonData;
using artdaq::database::ucon::UconDB;
using artdaq::database::ucon::DBConfig;

using artdaq::database::docrecord::JSONDocumentBuilder;
using artdaq::database::docrecord::JSONDocument;

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, UconDB>::findConfigurations(JsonData const& query_payload) {
  confirm(!query_payload.empty());
  auto returnCollection = std::list<JsonData>();

  TLOG(15) << "StorageProvider::FileSystemDB::findConfigurations() begin";
  TLOG(15) << "StorageProvider::FileSystemDB::findConfigurations() args data=<" << query_payload << ">";

  throw runtime_error("UconDB") << "StorageProvider::UconDB::findConfigurations() is not implemented";

  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, UconDB>::configurationComposition(JsonData const& query_payload) {
  confirm(!query_payload.empty());

  auto returnCollection = std::list<JsonData>();

  TLOG(16) << "StorageProvider::UconDB::configurationComposition() begin";
  TLOG(16) << "StorageProvider::UconDB::configurationComposition() args data=<" << query_payload << ">";

  throw runtime_error("UconDB") << "StorageProvider::UconDB::configurationComposition() is not implemented";

  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, UconDB>::findVersions(JsonData const& filter) {
  confirm(!filter.empty());

  auto returnCollection = std::list<JsonData>();

  TLOG(15) << "StorageProvider::UconDB::findVersions() begin";
  TLOG(15) << "StorageProvider::UconDB::findVersions() args data=<" << filter << ">";

  throw runtime_error("UconDB") << "StorageProvider::UconDB::findVersions() is not implemented";

  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, UconDB>::findEntities(JsonData const& filter) {
  confirm(!filter.empty());
  auto returnCollection = std::list<JsonData>();

  TLOG(19) << "StorageProvider::UconDB::findEntities() begin";
  TLOG(19) << "StorageProvider::UconDB::findEntities() args data=<" << filter << ">";

  throw runtime_error("UconDB") << "StorageProvider::UconDB::findEntities() is not implemented";

  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, UconDB>::listCollections(JsonData const& query_payload) {
  confirm(!query_payload.empty());

  auto returnCollection = std::list<JsonData>();
  TLOG(22) << "StorageProvider::UconDB::listCollections() begin";
  TLOG(22) << "StorageProvider::UconDB::listCollections() args data=<" << query_payload << ">";

  throw runtime_error("UconDB") << "StorageProvider::UconDB::listCollections() is not implemented";

  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, UconDB>::addConfiguration(JsonData const& query_payload) {
  confirm(!query_payload.empty());
  auto returnCollection = std::list<JsonData>();

  TLOG(15) << "StorageProvider::UconDB::addConfiguration() begin";
  TLOG(15) << "StorageProvider::UconDB::addConfiguration() args data=<" << query_payload << ">";

  throw runtime_error("UconDB") << "StorageProvider::UconDB::addConfiguration() is not implemented";

  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, UconDB>::listDatabases(JsonData const& query_payload) {
  confirm(!query_payload.empty());
  auto returnCollection = std::list<JsonData>();

  TLOG(19) << "StorageProvider::UconDB::listDatabases() begin";
  TLOG(19) << "StorageProvider::UconDB::listDatabases() args data=<" << query_payload << ">";

  throw runtime_error("UconDB") << "StorageProvider::UconDB::listDatabases() is not implemented";

  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, UconDB>::databaseMetadata(JsonData const& query_payload[[gnu::unused]]) {
  confirm(!query_payload.empty());
  auto returnCollection = std::list<JsonData>();

  throw runtime_error("UconDB") << "StorageProvider::UconDB::databaseMetadata() is not implemented";

  return returnCollection;
}

namespace ucon {
namespace debug {
void enable() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TLOG(10) <<  "artdaq::database::ucon trace_enable";
  artdaq::database::ucon::debug::UconDBAPI();
  artdaq::database::ucon::debug::ReadWrite();
}
}
}  // namespace ucon
}  // namespace database
}  // namespace artdaq
