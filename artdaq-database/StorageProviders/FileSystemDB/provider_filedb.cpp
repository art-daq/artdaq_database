#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h"
#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb_index.h"
#include "artdaq-database/StorageProviders/common.h"

#include "artdaq-database/SharedCommon/sharedcommon_common.h"

#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/BasicTypes/basictypes.h"

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


namespace artdaq {
namespace database {  
template <>
template <>
std::list<JsonData> StorageProvider<JsonData, FileSystemDB>::load(JsonData const& filter) {
  TRACE_(3, "StorageProvider::FileSystemDB::load() begin");
  TRACE_(3, "StorageProvider::FileSystemDB::load() args search=<" << filter.json_buffer << ">");

  auto search_filter_document = JSONDocument{filter.json_buffer};

  auto collection_name = search_filter_document.findChild("collection").value();
  auto search_filter = search_filter_document.findChild("filter").value();

  auto collection = _provider->connection() + collection_name;
  collection = expand_environment_variables(collection);

  TRACE_(3, "StorageProvider::FileSystemDB::load() collection_path=<" << collection << ">.");

  auto dir_name = dbfs::mkdir(collection);

  auto index_path = boost::filesystem::path(dir_name.c_str()).append(dbfsl::search_index);

  SearchIndex search_index(index_path);

  TRACE_(3, "StorageProvider::FileSystemDB::load() search_filter=<" << search_filter << ">.");

  auto oids = search_index.findDocumentIDs(search_filter);

  auto returnCollection = std::list<JsonData>();

  TRACE_(3, "StorageProvider::FileSystemDB::load() search returned " << oids.size() << " documents.");

  for (auto const& oid : oids) {
    auto doc_path = boost::filesystem::path(dir_name.c_str()).append(oid).replace_extension(".json");

    TRACE_(3, "StorageProvider::FileSystemDB::load() reading document <" << doc_path.c_str() << ">.");

    std::ifstream is(doc_path.c_str());
    std::string json((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());

    is.close();

    returnCollection.emplace_back(json);
  }

  return returnCollection;
}

template <>
object_id_t StorageProvider<JsonData, FileSystemDB>::store(JsonData const& data) {
  TRACE_(4, "StorageProvider::FileSystemDB::store() begin");
  TRACE_(4, "StorageProvider::FileSystemDB::store() args data=<" << data.json_buffer << ">");

  auto task_document = JSONDocument{data.json_buffer};
  auto collection_name = task_document.findChild("collection").value();

  auto oid = object_id_t{ouid_invalid};

  try {
    auto filter = task_document.findChild("filter").value();
    TRACE_(4, "StorageProvider::FileSystemDB::store() found filter=<" << filter << ">.");
    oid = extract_oid(filter);
    TRACE_(4, "StorageProvider::FileSystemDB::store() using provided oid=<" << oid << ">.");
  } catch (...) {
  }

  if (oid == object_id_t{ouid_invalid}) {
    oid = generate_oid();
  }

  std::ostringstream oss;
  oss << "{ \"_id\":\"" << oid << "\"}";
  
  
  auto document= task_document.findChildDocument("document");  
  JSONDocumentBuilder builder(document);
  builder.setObjectID({oss.str()});
  builder.setCollection(task_document);

  TRACE_(4, "StorageProvider::FileSystemDB::store() using generated oid=<" << oid << ">.");

  auto collection = _provider->connection() + collection_name;

  collection = expand_environment_variables(collection);

  TRACE_(4, "StorageProvider::FileSystemDB::store() collection_path=<" << collection << ">.");

  auto filename = dbfs::mkdir(collection) + oid + ".json";

  TRACE_(4, "StorageProvider::FileSystemDB::store() filename=<" << filename << ">.");

  std::ofstream os(filename);

  auto json = builder.to_string();

  std::copy(json.begin(), json.end(), std::ostream_iterator<char>(os));

  os.close();

  auto index_path = boost::filesystem::path(filename.c_str()).parent_path().append(dbfsl::search_index);

  SearchIndex search_index(index_path);

  if (!search_index.addDocument(json, oid)) {
    TRACE_(4, "StorageProvider::FileSystemDB Failed updating SearchIndex.");
  }

  return {"{ \"$oid\" : \"" + oid + "\"}"};
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, FileSystemDB>::findGlobalConfigs(JsonData const& search_filter) {
  confirm(!search_filter.json_buffer.empty());
  auto returnCollection = std::list<JsonData>();

  TRACE_(5, "StorageProvider::FileSystemDB::findGlobalConfigs() begin");
  TRACE_(5, "StorageProvider::FileSystemDB::findGlobalConfigs() args data=<" << search_filter.json_buffer << ">");

  auto collection = _provider->connection();
  collection = expand_environment_variables(collection);

  auto dir_name = dbfs::mkdir(collection);
  auto collection_names = dbfs::find_subdirs(dir_name);

  for (auto const& collection_name : collection_names) {
    TRACE_(5,
           "StorageProvider::FileSystemDB::findGlobalConfigs() querying "
           "collection_name=<"
               << collection_name << ">");

    auto index_path = boost::filesystem::path(dir_name.c_str()).append(collection_name).append(dbfsl::search_index);

    SearchIndex search_index(index_path);

    auto configentityname_pairs = search_index.findAllGlobalConfigurations(search_filter);

    TRACE_(5, "StorageProvider::FileSystemDB::findGlobalConfigs() search returned " << configentityname_pairs.size() << " configurations.");

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

      TRACE_(5, "StorageProvider::FileSystemDB::findGlobalConfigs() found document=<" << oss.str() << ">");

      returnCollection.emplace_back(oss.str());
    }
  }

  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, FileSystemDB>::buildConfigSearchFilter(JsonData const& search_filter) {
  confirm(!search_filter.json_buffer.empty());
  auto returnCollection = std::list<JsonData>();

  TRACE_(6, "StorageProvider::FileSystemDB::buildConfigSearchFilter() begin");
  TRACE_(6, "StorageProvider::FileSystemDB::buildConfigSearchFilter() args data=<" << search_filter.json_buffer << ">");

  auto collection = _provider->connection();
  collection = expand_environment_variables(collection);

  auto dir_name = dbfs::mkdir(collection);
  auto collection_names = dbfs::find_subdirs(dir_name);

  for (auto const& collection_name : collection_names) {
    TRACE_(6,
           "StorageProvider::FileSystemDB::buildConfigSearchFilter() "
           "querying collection_name=<"
               << collection_name << ">");

    auto index_path = boost::filesystem::path(dir_name.c_str()).append(collection_name).append(dbfsl::search_index);

    SearchIndex search_index(index_path);

    auto configentityname_pairs = search_index.findAllGlobalConfigurations(search_filter);

    TRACE_(6,
           "StorageProvider::FileSystemDB::buildConfigSearchFilter() search "
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
      oss << ", \"configurable_entity.name\" : \"" << configentityname_pair.second << "\"";
      oss << "}";
      oss << "}";

      TRACE_(6,
             "StorageProvider::FileSystemDB::buildConfigSearchFilter() "
             "found document=<"
                 << oss.str() << ">");

      returnCollection.emplace_back(oss.str());
    }
  }

  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, FileSystemDB>::findConfigVersions(JsonData const& filter) {
  confirm(!filter.json_buffer.empty());
  auto returnCollection = std::list<JsonData>();

  TRACE_(7, "StorageProvider::FileSystemDB::findConfigVersions() begin");
  TRACE_(7, "StorageProvider::FileSystemDB::findConfigVersions() args data=<" << filter.json_buffer << ">");

  auto search_filter_document = JSONDocument{filter.json_buffer};

  auto collection_name = search_filter_document.findChild("collection").value();
  auto search_filter = search_filter_document.findChild("filter").value();

  auto collection = _provider->connection() + collection_name;
  collection = expand_environment_variables(collection);

  TRACE_(7, "StorageProvider::FileSystemDB::load() collection_path=<" << collection << ">.");

  auto dir_name = dbfs::mkdir(collection);

  auto index_path = boost::filesystem::path(dir_name.c_str()).append(dbfsl::search_index);

  SearchIndex search_index(index_path);

  jsn::object_t search_ast;

  if (!jsn::JsonReader{}.read(search_filter, search_ast)) {
    TRACE_(5, "StorageProvider::FileSystemDB::index::findVersionsByEntityName()"
                  << " Failed to create an AST from search.");
    return returnCollection;
  }

  if (search_ast.count("configurations.name") == 0) {
    auto versionentityname_pairs = search_index.findVersionsByEntityName(search_filter);

    TRACE_(7,
           "StorageProvider::FileSystemDB::findVersionsByEntityName() "
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
      oss << ", \"configurable_entity.name\" : \"" << versionentityname_pair.first << "\"";
      oss << "}";
      oss << "}";

      TRACE_(7, "StorageProvider::FileSystemDB::findConfigVersions() found document=<" << oss.str() << ">");

      returnCollection.emplace_back(oss.str());
    }
  } else {
    auto entityName = std::string{"notprovided"};

    try {
      entityName = boost::get<std::string>(search_ast.at("configurable_entity.name"));

      TRACE_(7, "StorageProvider::FileSystemDB::findVersionsByGlobalConfigName"
                    << " Found entity filter=<" << entityName << ">.");

    } catch (...) {
    }

    auto versionentityname_pairs = search_index.findVersionsByGlobalConfigName(search_filter);

    TRACE_(7,
           "StorageProvider::FileSystemDB::findVersionsByGlobalConfigName() "
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
      oss << ", \"configurable_entity.name\" : \"" << entityName << "\"";
      oss << "}";
      oss << "}";

      TRACE_(7, "StorageProvider::FileSystemDB::findConfigVersions() found document=<" << oss.str() << ">");

      returnCollection.emplace_back(oss.str());
    }
  }
  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, FileSystemDB>::findConfigEntities(JsonData const& filter) {
  confirm(!filter.json_buffer.empty());
  auto returnCollection = std::list<JsonData>();

  TRACE_(9, "StorageProvider::FileSystemDB::findConfigEntities() begin");
  TRACE_(9, "StorageProvider::FileSystemDB::findConfigEntities() args data=<" << filter.json_buffer << ">");

  auto search_filter_document = JSONDocument{filter.json_buffer};
  auto search_filter = search_filter_document.findChild("filter").value();

  auto collection = _provider->connection();
  collection = expand_environment_variables(collection);

  auto dir_name = dbfs::mkdir(collection);
  auto collection_names = dbfs::find_subdirs(dir_name);

  for (auto const& collection_name : collection_names) {
    TRACE_(9,
           "StorageProvider::FileSystemDB::findConfigEntities() querying "
           "collection_name=<"
               << collection_name << ">");

    auto index_path = boost::filesystem::path(dir_name.c_str()).append(collection_name).append(dbfsl::search_index);

    SearchIndex search_index(index_path);

    auto configentity_names = search_index.findConfigEntities(search_filter);

    if (configentity_names.size() > 1) {
      std::sort(configentity_names.begin(), configentity_names.end());
      auto unique_configentity_names = std::vector<std::string>{};

      std::unique_copy(configentity_names.begin(), configentity_names.end(), back_inserter(unique_configentity_names));
      configentity_names.swap(unique_configentity_names);
    }

    TRACE_(9, "StorageProvider::FileSystemDB::findConfigEntities() search returned " << configentity_names.size() << " configurations.");

    for (auto const& configentity_name : configentity_names) {
      std::ostringstream oss;

      oss << "{";
      oss << "\"collection\" : \"" << collection_name << "\",";
      oss << "\"dbprovider\" : \"filesystem\",";
      oss << "\"dataformat\" : \"gui\",";
      oss << "\"operation\" : \"findversions\",";
      oss << "\"filter\" : {";
      oss << "\"configurable_entity.name\" : \"" << configentity_name << "\"";
      oss << "}";
      oss << "}";

      TRACE_(9, "StorageProvider::FileSystemDB::findConfigEntities() found document=<" << oss.str() << ">");

      returnCollection.emplace_back(oss.str());
    }
  }

  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, FileSystemDB>::listCollectionNames(JsonData const& search_filter) {
  confirm(!search_filter.json_buffer.empty());
  auto returnCollection = std::list<JsonData>();
  TRACE_(12, "StorageProvider::FileSystemDB::listCollectionNames() begin");
  TRACE_(12, "StorageProvider::FileSystemDB::listCollectionNames() args data=<" << search_filter.json_buffer << ">");

  auto collection = _provider->connection();
  collection = expand_environment_variables(collection);

  auto dir_name = dbfs::mkdir(collection);
  auto collection_names = dbfs::find_subdirs(dir_name);

  for (auto const& collection_name : collection_names) {
    TRACE_(12,
           "StorageProvider::FileSystemDB::listCollectionNames() found "
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

    TRACE_(12, "StorageProvider::FileSystemDB::listCollectionNames() found document=<" << oss.str() << ">");

    returnCollection.emplace_back(oss.str());
  }
  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, FileSystemDB>::addConfigToGlobalConfig(JsonData const& search_filter) {
  confirm(!search_filter.json_buffer.empty());
  auto returnCollection = std::list<JsonData>();

  TRACE_(8, "StorageProvider::FileSystemDB::addConfigToGlobalConfig() begin");
  TRACE_(8, "StorageProvider::FileSystemDB::addConfigToGlobalConfig() args data=<" << search_filter.json_buffer << ">");

  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, FileSystemDB>::listDatabaseNames(JsonData const& search_filter) {
  confirm(!search_filter.json_buffer.empty());
  auto returnCollection = std::list<JsonData>();

  TRACE_(9, "StorageProvider::FileSystemDB::listDatabaseNames() begin");
  TRACE_(9, "StorageProvider::FileSystemDB::listDatabaseNames() args data=<" << search_filter.json_buffer << ">");

  auto database = _provider->connection();

  auto found = database.find_last_not_of("\\/");
  if (found != std::string::npos) database.erase(found + 1);

  auto database_names = dbfs::find_siblingdirs(database);

  for (auto const& database_name : database_names) {
    TRACE_(9, "StorageProvider::FileSystemDB::listDatabaseNames() found databases=<" << database_name << ">");

    if (database_name == "system") continue;

    TRACE_(9, "StorageProvider::FileSystemDB::listDatabaseNames() found database_name=<" << database_name << ">");

    std::ostringstream oss;
    oss << "{";
    oss << "\"database\" : \"" << database_name << "\",";
    oss << "\"dbprovider\" : \"filesystem\",";
    oss << "\"dataformat\" : \"gui\",";
    oss << "\"filter\" : {}";
    oss << "}";

    TRACE_(9, "StorageProvider::FileSystemDB::listDatabaseNames() found document=<" << oss.str() << ">");

    returnCollection.emplace_back(oss.str());
  }

  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, FileSystemDB>::databaseMetadata(JsonData const& search_filter [[gnu::unused]]) {
  confirm(!search_filter.json_buffer.empty());
  auto returnCollection = std::list<JsonData>();

  auto collection = _provider->connection() + system_metadata;
  collection = expand_environment_variables(collection);

  if (collection.find(dbfsl::FILEURI) == 0) collection = collection.substr(strlen(dbfsl::FILEURI));

  TRACE_(3, "StorageProvider::FileSystemDB::databaseMetadata() collection_path=<" << collection << ">.");

  auto oids = dbfs::find_documents(collection);

  TRACE_(3, "StorageProvider::FileSystemDB::databaseMetadata() search returned " << oids.size() << " documents.");

  for (auto const& oid : oids) {
    auto doc_path = boost::filesystem::path(collection).append(oid).replace_extension(".json");

    TRACE_(3, "StorageProvider::FileSystemDB::databaseMetadata() reading document <" << doc_path.c_str() << ">.");

    std::ifstream is(doc_path.c_str());
    std::string json((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());

    is.close();

    TRACE_(3, "StorageProvider::FileSystemDB::databaseMetadata() document <" << json << ">.");

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

  artdaq::database::filesystem::index::debug::enable();
}
}
}  // namespace filesystem


}  // namespace database
}  // namespace artdaq
