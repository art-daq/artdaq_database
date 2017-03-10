#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h"
#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb_index.h"
#include "artdaq-database/StorageProviders/common.h"

#include "artdaq-database/SharedCommon/sharedcommon_common.h"

#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"

/*
#include <wordexp.h>
#include <boost/filesystem.hpp>
#include <boost/range/adaptors.hpp>
*/

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "PRVDR:FileDB_C"

namespace db = artdaq::database;
namespace dbfs = artdaq::database::filesystem;
namespace dbfsl = dbfs::literal;

using artdaq::database::filesystem::DBConfig;
using artdaq::database::filesystem::FileSystemDB;
using artdaq::database::filesystem::index::SearchIndex;

using artdaq::database::basictypes::JsonData;
using artdaq::database::docrecord::JSONDocumentBuilder;
using artdaq::database::docrecord::JSONDocument;

namespace jsonliteral = artdaq::database::dataformats::literal;

namespace artdaq {
namespace database {
template <>
template <>
std::list<JsonData> StorageProvider<JsonData, FileSystemDB>::findConfigurations(JsonData const& query_payload) {
  confirm(!query_payload.empty());
  auto returnCollection = std::list<JsonData>();

  TRACE_(5, "FileSystemDB::findConfigurations() begin");
  TRACE_(5, "FileSystemDB::findConfigurations() args data=<" << query_payload << ">");

  auto collection = _provider->connection();
  collection = expand_environment_variables(collection);

  auto dir_name = dbfs::mkdir(collection).append("/");
  auto collection_names = dbfs::find_subdirs(dir_name);

  for (auto const& collection_name : collection_names) {
    TRACE_(5,
           "FileSystemDB::findConfigurations() querying "
           "collection_name=<"
               << collection_name << ">");

    auto index_path = boost::filesystem::path(dir_name.c_str()).append(collection_name).append(dbfsl::search_index);

    SearchIndex search_index(index_path);

    auto configentityname_pairs = search_index.findAllGlobalConfigurations(query_payload);

    TRACE_(5, "FileSystemDB::findConfigurations() search returned " << configentityname_pairs.size()
                                                                    << " configurations.");

    for (auto const& configentityname_pair : configentityname_pairs) {
      std::ostringstream oss;

      oss << "{";
      // oss << "\"collection\" : \"" << collection_name << "\",";
      oss << "\"dbprovider\" : \"filesystem\",";
      oss << "\"dataformat\" : \"gui\",";
      oss << "\"operation\" : \"buildfilter\",";
      oss << "\"filter\" : {";
      oss << "\"configurations.name\" : \"" << configentityname_pair.first << "\"";
      oss << "}";
      oss << "}";

      TRACE_(5, "FileSystemDB::findConfigurations() found document=<" << oss.str() << ">");

      returnCollection.emplace_back(oss.str());
    }
  }

  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, FileSystemDB>::configurationComposition(JsonData const& query_payload) {
  confirm(!query_payload.empty());
  auto returnCollection = std::list<JsonData>();

  TRACE_(6, "FileSystemDB::configurationComposition() begin");
  TRACE_(6, "FileSystemDB::configurationComposition() args data=<" << query_payload << ">");

  auto collection = _provider->connection();
  collection = expand_environment_variables(collection);

  auto dir_name = dbfs::mkdir(collection).append("/");
  auto collection_names = dbfs::find_subdirs(dir_name);

  for (auto const& collection_name : collection_names) {
    TRACE_(6,
           "FileSystemDB::configurationComposition() "
           "querying collection_name=<"
               << collection_name << ">");

    auto index_path = boost::filesystem::path(dir_name.c_str()).append(collection_name).append(dbfsl::search_index);

    SearchIndex search_index(index_path);

    auto configentityname_pairs = search_index.findAllGlobalConfigurations(query_payload);

    TRACE_(6,
           "FileSystemDB::configurationComposition() search "
           "returned "
               << configentityname_pairs.size() << " configurations.");

    for (auto const& configentityname_pair : configentityname_pairs) {
      std::ostringstream oss;

      oss << "{";
      oss << "\"collection\" : \"" << collection_name << "\",";
      oss << "\"dbprovider\" : \"filesystem\",";
      oss << "\"dataformat\" : \"gui\",";
      oss << "\"operation\" : \"load\",";
      oss << "\"filter\" : {";
      oss << "\"configurations.name\" : \"" << configentityname_pair.first << "\"";
      oss << ", \"entities.name\" : \"" << configentityname_pair.second << "\"";
      oss << "}";
      oss << "}";

      TRACE_(6,
             "FileSystemDB::configurationComposition() "
             "found document=<"
                 << oss.str() << ">");

      returnCollection.emplace_back(oss.str());
    }
  }

  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, FileSystemDB>::findVersions(JsonData const& filter) {
  confirm(!filter.empty());
  auto returnCollection = std::list<JsonData>();

  TRACE_(7, "FileSystemDB::findVersions() begin");
  TRACE_(7, "FileSystemDB::findVersions() args data=<" << filter << ">");

  auto query_payload_document = JSONDocument{filter};

  auto collection_name = query_payload_document.findChild("collection").value();
  auto query_payload = query_payload_document.findChild("filter").value();

  auto collection = _provider->connection() + collection_name;
  collection = expand_environment_variables(collection);

  TRACE_(7, "FileSystemDB::readDocument() collection_path=<" << collection << ">.");

  auto dir_name = dbfs::mkdir(collection).append("/");

  auto index_path = boost::filesystem::path(dir_name.c_str()).append(dbfsl::search_index);

  SearchIndex search_index(index_path);

  jsn::object_t search_ast;

  if (!jsn::JsonReader{}.read(query_payload, search_ast)) {
    TRACE_(5, "FileSystemDB::index::findVersionsByEntityName()"
                  << " Failed to create an AST from search.");
    return returnCollection;
  }

  if (search_ast.count("configurations.name") == 0) {
    auto versionentityname_pairs = search_index.findVersionsByEntityName(query_payload);

    TRACE_(7,
           "FileSystemDB::findVersionsByEntityName() "
           "search returned "
               << versionentityname_pairs.size() << " configurations.");

    for (auto const& versionentityname_pair : versionentityname_pairs) {
      std::ostringstream oss;

      oss << "{";
      oss << "\"collection\" : \"" << collection_name << "\",";
      oss << "\"dbprovider\" : \"filesystem\",";
      oss << "\"dataformat\" : \"gui\",";
      oss << "\"operation\" : \"load\",";
      oss << "\"filter\" : {";
      oss << "\"version\" : \"" << versionentityname_pair.second << "\"";
      oss << ", \"entities.name\" : \"" << versionentityname_pair.first << "\"";
      oss << "}";
      oss << "}";

      TRACE_(7, "FileSystemDB::findVersions() found document=<" << oss.str() << ">");

      returnCollection.emplace_back(oss.str());
    }
  } else {
    auto entityName = std::string{"notprovided"};

    try {
      entityName = boost::get<std::string>(search_ast.at("entities.name"));

      TRACE_(7, "FileSystemDB::findVersionsByGlobalConfigName"
                    << " Found entity filter=<" << entityName << ">.");

    } catch (...) {
    }

    auto versionentityname_pairs = search_index.findVersionsByGlobalConfigName(query_payload);

    TRACE_(7,
           "FileSystemDB::findVersionsByGlobalConfigName() "
           "search returned "
               << versionentityname_pairs.size() << " configurations.");

    for (auto const& versionentityname_pair : versionentityname_pairs) {
      std::ostringstream oss;

      oss << "{";
      oss << "\"collection\" : \"" << collection_name << "\",";
      oss << "\"dbprovider\" : \"filesystem\",";
      oss << "\"dataformat\" : \"gui\",";
      oss << "\"operation\" : \"load\",";
      oss << "\"filter\" : {";
      oss << "\"version\" : \"" << versionentityname_pair.second << "\"";
      oss << ", \"entities.name\" : \"" << entityName << "\"";
      oss << "}";
      oss << "}";

      TRACE_(7, "FileSystemDB::findVersions() found document=<" << oss.str() << ">");

      returnCollection.emplace_back(oss.str());
    }
  }
  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, FileSystemDB>::findEntities(JsonData const& filter) {
  confirm(!filter.empty());
  auto returnCollection = std::list<JsonData>();

  TRACE_(9, "FileSystemDB::findEntities() begin");
  TRACE_(9, "FileSystemDB::findEntities() args data=<" << filter << ">");

  auto query_payload_document = JSONDocument{filter};
  auto query_payload = query_payload_document.findChild("filter").value();

  auto collection = _provider->connection();
  collection = expand_environment_variables(collection);

  auto dir_name = dbfs::mkdir(collection).append("/");
  auto collection_names = dbfs::find_subdirs(dir_name);

  for (auto const& collection_name : collection_names) {
    TRACE_(9,
           "FileSystemDB::findEntities() querying "
           "collection_name=<"
               << collection_name << ">");

    auto index_path = boost::filesystem::path(dir_name.c_str()).append(collection_name).append(dbfsl::search_index);

    SearchIndex search_index(index_path);

    auto configentity_names = search_index.findEntities(query_payload);

    if (configentity_names.size() > 1) {
      std::sort(configentity_names.begin(), configentity_names.end());
      auto unique_configentity_names = std::vector<std::string>{};

      std::unique_copy(configentity_names.begin(), configentity_names.end(), back_inserter(unique_configentity_names));
      configentity_names.swap(unique_configentity_names);
    }

    TRACE_(9, "FileSystemDB::findEntities() search returned " << configentity_names.size() << " configurations.");

    for (auto const& configentity_name : configentity_names) {
      std::ostringstream oss;

      oss << "{";
      oss << "\"collection\" : \"" << collection_name << "\",";
      oss << "\"dbprovider\" : \"filesystem\",";
      oss << "\"dataformat\" : \"gui\",";
      oss << "\"operation\" : \"findversions\",";
      oss << "\"filter\" : {";
      oss << "\"entities.name\" : \"" << configentity_name << "\"";
      oss << "}";
      oss << "}";

      TRACE_(9, "FileSystemDB::findEntities() found document=<" << oss.str() << ">");

      returnCollection.emplace_back(oss.str());
    }
  }

  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, FileSystemDB>::listCollections(JsonData const& query_payload) {
  confirm(!query_payload.empty());
  auto returnCollection = std::list<JsonData>();
  TRACE_(12, "FileSystemDB::listCollections() begin");
  TRACE_(12, "FileSystemDB::listCollections() args data=<" << query_payload << ">");

  auto collection = _provider->connection();
  collection = expand_environment_variables(collection);

  auto dir_name = dbfs::mkdir(collection).append("/");
  auto collection_names = dbfs::find_subdirs(dir_name);

  for (auto const& collection_name : collection_names) {
    if (collection_name == "system.indexes" || collection_name == system_metadata) continue;

    TRACE_(12,
           "FileSystemDB::listCollections() found "
           "collection_name=<"
               << collection_name << ">");

    std::ostringstream oss;

    oss << "{";
    oss << "\"collection\" : \"" << collection_name << "\",";
    oss << "\"dbprovider\" : \"filesystem\",";
    oss << "\"dataformat\" : \"gui\",";
    oss << "\"operation\" : \"findversions\",";
    oss << "\"filter\" : { }";
    oss << "}";

    TRACE_(12, "FileSystemDB::listCollections() found document=<" << oss.str() << ">");

    returnCollection.emplace_back(oss.str());
  }
  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, FileSystemDB>::addConfiguration(JsonData const& query_payload) {
  confirm(!query_payload.empty());
  auto returnCollection = std::list<JsonData>();

  TRACE_(8, "FileSystemDB::addConfiguration() begin");
  TRACE_(8, "FileSystemDB::addConfiguration() args data=<" << query_payload << ">");

  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, FileSystemDB>::listDatabases(JsonData const& query_payload) {
  confirm(!query_payload.empty());
  auto returnCollection = std::list<JsonData>();

  TRACE_(9, "FileSystemDB::listDatabases() begin");
  TRACE_(9, "FileSystemDB::listDatabases() args data=<" << query_payload << ">");

  auto database = _provider->connection();

  auto found = database.find_last_not_of("\\/");
  if (found != std::string::npos) database.erase(found + 1);

  auto database_names = dbfs::find_siblingdirs(database);

  for (auto const& database_name : database_names) {
    TRACE_(9, "FileSystemDB::listDatabases() found databases=<" << database_name << ">");

    if (database_name == "system") continue;

    TRACE_(9, "FileSystemDB::listDatabases() found database_name=<" << database_name << ">");

    std::ostringstream oss;
    oss << "{";
    oss << "\"database\" : \"" << database_name << "\",";
    oss << "\"dbprovider\" : \"filesystem\",";
    oss << "\"dataformat\" : \"gui\",";
    oss << "\"filter\" : {}";
    oss << "}";

    TRACE_(9, "FileSystemDB::listDatabases() found document=<" << oss.str() << ">");

    returnCollection.emplace_back(oss.str());
  }

  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, FileSystemDB>::databaseMetadata(
    JsonData const& query_payload[[gnu::unused]]) {
  confirm(!query_payload.empty());
  auto returnCollection = std::list<JsonData>();

  auto collection = _provider->connection() + system_metadata;
  collection = expand_environment_variables(collection);

  if (collection.find(dbfsl::FILEURI) == 0) collection = collection.substr(strlen(dbfsl::FILEURI));

  TRACE_(3, "FileSystemDB::databaseMetadata() collection_path=<" << collection << ">.");

  auto oids = dbfs::find_documents(collection);

  TRACE_(3, "FileSystemDB::databaseMetadata() search returned " << oids.size() << " documents.");

  for (auto const& oid : oids) {
    auto doc_path = boost::filesystem::path(collection).append(oid).replace_extension(".json");

    TRACE_(3, "FileSystemDB::databaseMetadata() reading document <" << doc_path.c_str() << ">.");

    auto json = std::string{};

    db::read_buffer_from_file(json, {doc_path.c_str()});

    TRACE_(3, "FileSystemDB::databaseMetadata() document <" << json << ">.");

    returnCollection.emplace_back(json);
  }

  return returnCollection;
}

namespace filesystem {
namespace debug {
void enable() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TRACE_(0, "artdaq::database::filesystem trace_enable");
  artdaq::database::filesystem::debug::ReadWrite();
  artdaq::database::filesystem::index::debug::enable();
}
}
}  // namespace filesystem

}  // namespace database
}  // namespace artdaq
