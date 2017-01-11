#include "artdaq-database/StorageProviders/UcondDB/provider_uconddb.h"
#include "artdaq-database/StorageProviders/common.h"

#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"

#include "artdaq-database/BasicTypes/basictypes.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "PRVDR:UcondDB_C"

namespace db = artdaq::database;
namespace dbfsl = db::ucond::literal;

namespace artdaq {
namespace database {

using artdaq::database::basictypes::JsonData;
using artdaq::database::ucond::UcondDB;

using artdaq::database::docrecord::JSONDocumentBuilder;
using artdaq::database::docrecord::JSONDocument;

object_id_t generate_oid();
object_id_t extract_oid(std::string const&);

std::string expand_environment_variables(std::string var);

std::string& UcondDB::connection() { return _connection; }

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, UcondDB>::load(JsonData const& filter) {
  TRACE_(3, "StorageProvider::UcondDB::load() begin");
  TRACE_(3, "StorageProvider::UcondDB::load() args search=<" << filter.json_buffer << ">");
  auto returnCollection = std::list<JsonData>();

  throw cet::exception("UcondDB") << "StorageProvider::UcondDB::load() is not implemented";

  return returnCollection;
}

template <>
object_id_t StorageProvider<JsonData, UcondDB>::store(JsonData const& data) {
  TRACE_(4, "StorageProvider::FileSystemDB::store() begin");
  TRACE_(4, "StorageProvider::FileSystemDB::store() args data=<" << data.json_buffer << ">");

  JSONDocumentBuilder builder{data.json_buffer};

  auto collection_name = builder.extract().findChild("collection").value();

  auto oid = object_id_t{ouid_invalid};

  try {
    auto filter = builder.extract().findChild("filter").value();
    TRACE_(4, "StorageProvider::FileSystemDB::store() found filter=<" << filter << ">.");
    oid = extract_oid(filter);
    TRACE_(4, "StorageProvider::FileSystemDB::store() using provided oid=<" << oid << ">.");
  } catch (...) {
  }

  if (oid == object_id_t{ouid_invalid}) {
    oid = generate_oid();
  }

  throw cet::exception("UcondDB") << "StorageProvider::UcondDB::store() is not implemented";

  return {"{ \"$oid\" : \"" + oid + "\"}"};
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, UcondDB>::findGlobalConfigs(JsonData const& search_filter) {
  assert(!search_filter.json_buffer.empty());
  auto returnCollection = std::list<JsonData>();

  TRACE_(5, "StorageProvider::FileSystemDB::findGlobalConfigs() begin");
  TRACE_(5, "StorageProvider::FileSystemDB::findGlobalConfigs() args data=<" << search_filter.json_buffer << ">");

  throw cet::exception("UcondDB") << "StorageProvider::UcondDB::findGlobalConfigs() is not implemented";

  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, UcondDB>::buildConfigSearchFilter(JsonData const& search_filter) {
  assert(!search_filter.json_buffer.empty());
  auto returnCollection = std::list<JsonData>();

  TRACE_(6, "StorageProvider::UcondDB::buildConfigSearchFilter() begin");
  TRACE_(6, "StorageProvider::UcondDB::buildConfigSearchFilter() args data=<" << search_filter.json_buffer << ">");

  throw cet::exception("UcondDB") << "StorageProvider::UcondDB::buildConfigSearchFilter() is not implemented";

  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, UcondDB>::findConfigVersions(JsonData const& filter) {
  assert(!filter.json_buffer.empty());
  auto returnCollection = std::list<JsonData>();

  TRACE_(7, "StorageProvider::UcondDB::findConfigVersions() begin");
  TRACE_(7, "StorageProvider::UcondDB::findConfigVersions() args data=<" << filter.json_buffer << ">");

  throw cet::exception("UcondDB") << "StorageProvider::UcondDB::findConfigVersions() is not implemented";

  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, UcondDB>::findConfigEntities(JsonData const& filter) {
  assert(!filter.json_buffer.empty());
  auto returnCollection = std::list<JsonData>();

  TRACE_(9, "StorageProvider::UcondDB::findConfigEntities() begin");
  TRACE_(9, "StorageProvider::UcondDB::findConfigEntities() args data=<" << filter.json_buffer << ">");

  throw cet::exception("UcondDB") << "StorageProvider::UcondDB::findConfigEntities() is not implemented";

  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, UcondDB>::listCollectionNames(JsonData const& search_filter) {
  assert(!search_filter.json_buffer.empty());
  auto returnCollection = std::list<JsonData>();
  TRACE_(12, "StorageProvider::UcondDB::listCollectionNames() begin");
  TRACE_(12, "StorageProvider::UcondDB::listCollectionNames() args data=<" << search_filter.json_buffer << ">");

  throw cet::exception("UcondDB") << "StorageProvider::UcondDB::listCollectionNames() is not implemented";

  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, UcondDB>::addConfigToGlobalConfig(JsonData const& search_filter) {
  assert(!search_filter.json_buffer.empty());
  auto returnCollection = std::list<JsonData>();

  TRACE_(8, "StorageProvider::UcondDB::addConfigToGlobalConfig() begin");
  TRACE_(8, "StorageProvider::UcondDB::addConfigToGlobalConfig() args data=<" << search_filter.json_buffer << ">");

  throw cet::exception("UcondDB") << "StorageProvider::UcondDB::addConfigToGlobalConfig() is not implemented";

  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, UcondDB>::listDatabaseNames(JsonData const& search_filter) {
  assert(!search_filter.json_buffer.empty());
  auto returnCollection = std::list<JsonData>();

  TRACE_(9, "StorageProvider::UcondDB::listDatabaseNames() begin");
  TRACE_(9, "StorageProvider::UcondDB::listDatabaseNames() args data=<" << search_filter.json_buffer << ">");

  throw cet::exception("UcondDB") << "StorageProvider::UcondDB::listDatabaseNames() is not implemented";

  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, UcondDB>::databaseMetadata(JsonData const& search_filter) {
  assert(!search_filter.json_buffer.empty());
  auto returnCollection = std::list<JsonData>();

  throw cet::exception("UcondDB") << "StorageProvider::UcondDB::databaseMetadata() is not implemented";

  return returnCollection;
}

namespace ucond {
namespace debug {
void enable() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TRACE_(0, "artdaq::database::ucond trace_enable");
}
}
}  // namespace ucond
}  // namespace database
}  // namespace artdaq
