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
  confirm(!query_payload.json_buffer.empty());
  auto returnCollection = std::list<JsonData>();

  TRACE_(5, "StorageProvider::FileSystemDB::findConfigurations() begin");
  TRACE_(5, "StorageProvider::FileSystemDB::findConfigurations() args data=<" << query_payload.json_buffer << ">");

  throw runtime_error("UconDB") << "StorageProvider::UconDB::findConfigurations() is not implemented";

  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, UconDB>::configurationComposition(JsonData const& query_payload) {
  confirm(!query_payload.json_buffer.empty());
  
  auto returnCollection = std::list<JsonData>();

  TRACE_(6, "StorageProvider::UconDB::configurationComposition() begin");
  TRACE_(6, "StorageProvider::UconDB::configurationComposition() args data=<" << query_payload.json_buffer << ">");

  throw runtime_error("UconDB") << "StorageProvider::UconDB::configurationComposition() is not implemented";

  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, UconDB>::findVersions(JsonData const& filter) {
  confirm(!filter.json_buffer.empty());
  
  auto returnCollection = std::list<JsonData>();

  TRACE_(7, "StorageProvider::UconDB::findVersions() begin");
  TRACE_(7, "StorageProvider::UconDB::findVersions() args data=<" << filter.json_buffer << ">");

  throw runtime_error("UconDB") << "StorageProvider::UconDB::findVersions() is not implemented";

  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, UconDB>::findEntities(JsonData const& filter) {
  confirm(!filter.json_buffer.empty());
  auto returnCollection = std::list<JsonData>();

  TRACE_(9, "StorageProvider::UconDB::findEntities() begin");
  TRACE_(9, "StorageProvider::UconDB::findEntities() args data=<" << filter.json_buffer << ">");

  throw runtime_error("UconDB") << "StorageProvider::UconDB::findEntities() is not implemented";

  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, UconDB>::listCollections(JsonData const& query_payload) {
  confirm(!query_payload.json_buffer.empty());

  auto returnCollection = std::list<JsonData>();
  TRACE_(12, "StorageProvider::UconDB::listCollections() begin");
  TRACE_(12, "StorageProvider::UconDB::listCollections() args data=<" << query_payload.json_buffer << ">");

  throw runtime_error("UconDB") << "StorageProvider::UconDB::listCollections() is not implemented";

  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, UconDB>::addConfiguration(JsonData const& query_payload) {
  confirm(!query_payload.json_buffer.empty());
  auto returnCollection = std::list<JsonData>();

  TRACE_(8, "StorageProvider::UconDB::addConfiguration() begin");
  TRACE_(8, "StorageProvider::UconDB::addConfiguration() args data=<" << query_payload.json_buffer << ">");

  throw runtime_error("UconDB") << "StorageProvider::UconDB::addConfiguration() is not implemented";

  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, UconDB>::listDatabases(JsonData const& query_payload) {
  confirm(!query_payload.json_buffer.empty());
  auto returnCollection = std::list<JsonData>();

  TRACE_(9, "StorageProvider::UconDB::listDatabases() begin");
  TRACE_(9, "StorageProvider::UconDB::listDatabases() args data=<" << query_payload.json_buffer << ">");

  throw runtime_error("UconDB") << "StorageProvider::UconDB::listDatabases() is not implemented";

  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, UconDB>::databaseMetadata(JsonData const& query_payload [[gnu::unused]]) {
  confirm(!query_payload.json_buffer.empty());
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

  TRACE_(0, "artdaq::database::ucon trace_enable");
  artdaq::database::ucon::debug::enableUconDBAPI();
  artdaq::database::ucon::debug::enableReadWrite();   
}
}
}  // namespace ucon
}  // namespace database
}  // namespace artdaq