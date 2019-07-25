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

#define TRACE_NAME "provider_filedb.cpp"

namespace db = artdaq::database;
namespace dbfs = artdaq::database::filesystem;
namespace dbfsl = dbfs::literal;

using artdaq::database::filesystem::FileSystemDB;
using artdaq::database::filesystem::index::SearchIndex;

using artdaq::database::docrecord::JSONDocument;

namespace apiliteral = db::configapi::literal;

namespace artdaq {
namespace database {
template <>
template <>
std::vector<JSONDocument> StorageProvider<JSONDocument, FileSystemDB>::findConfigurations(JSONDocument const& query_payload) {
  confirm(!query_payload.empty());
  auto returnCollection = std::vector<JSONDocument>();

  using timestamp_t = unsigned long long;
  using ordered_timestamps_t = std::set<timestamp_t>;
  using config_timestamps_t = std::map<std::string, ordered_timestamps_t>;
  auto config_timestamps = config_timestamps_t();

  auto reverse_timestamp_cmp = [](const timestamp_t& a, const timestamp_t& b) { return a > b; };
  using timestamp_configs_t = std::multimap<timestamp_t, std::string, decltype(reverse_timestamp_cmp)>;

  auto timestamp_configs = timestamp_configs_t(reverse_timestamp_cmp);

  TLOG(15) << "FileSystemDB::findConfigurations() begin";
  TLOG(15) << "FileSystemDB::findConfigurations() args data=<" << query_payload << ">";

  auto collection = _provider->connection();
  collection = expand_environment_variables(collection);

  auto dir_name = dbfs::mkdir(collection).append("/");
  auto collection_names = dbfs::find_subdirs(dir_name);

  for (auto const& collection_name : collection_names) {
    TLOG(15) << "FileSystemDB::findConfigurations() querying "
                "collection_name=<"
             << collection_name << ">";

    auto index_path = boost::filesystem::path(dir_name.c_str()).append(collection_name).append(dbfsl::search_index);

    SearchIndex search_index(index_path);

    auto configentityname_pairs = search_index.findAllGlobalConfigurations(query_payload);

    TLOG(15) << "FileSystemDB::findConfigurations() search returned " << configentityname_pairs.size() << " configurations.";
    for (auto const& configentityname_pair : configentityname_pairs) {
      auto const& name = configentityname_pair.first;
      std::ostringstream oss;
      oss << "{" << db::quoted_(apiliteral::filter::configurations) << ":" << db::quoted_(name) << "}";
      auto timestamps = search_index.getConfigurationAssignedTimestamps(oss.str());
      for (auto const& assigned : timestamps) {
        config_timestamps[name].insert(std::chrono::duration_cast<std::chrono::seconds>(db::to_timepoint(assigned).time_since_epoch()).count());
      }
    }
  }

  for (auto const& cfg : config_timestamps) {
    timestamp_configs.emplace(*cfg.second.rbegin(), cfg.first);
  }

  // keys are sorted the reverse chronological order
  for (auto const& cfg : timestamp_configs) {
    std::ostringstream oss;
    oss << "{";
    // oss << db::quoted_(apiliteral::option::collection) <<":" << db::quoted_(collection_name) << ",";

    oss << db::quoted_(apiliteral::option::provider) << ":" << db::quoted_(apiliteral::provider::filesystem) << ",";
    oss << db::quoted_(apiliteral::option::format) << ":" << db::quoted_(apiliteral::format::gui) << ",";
    oss << db::quoted_(apiliteral::option::operation) << ":" << db::quoted_(apiliteral::operation::confcomposition) << ",";
    oss << db::quoted_(apiliteral::option::searchfilter) << ":"
        << "{";
    oss << db::quoted_(apiliteral::filter::configurations) << ": " << db::quoted_(cfg.second);
    oss << "}";
    oss << "}";
    TLOG(15) << "FileSystemDB::findConfigurations() found document=<" << oss.str() << ">";

    returnCollection.emplace_back(oss.str());
  }

  return returnCollection;
}

template <>
template <>
std::vector<JSONDocument> StorageProvider<JSONDocument, FileSystemDB>::configurationComposition(JSONDocument const& query_payload) {
  confirm(!query_payload.empty());
  auto returnCollection = std::vector<JSONDocument>();

  TLOG(16) << "FileSystemDB::configurationComposition() begin";
  TLOG(16) << "FileSystemDB::configurationComposition() args data=<" << query_payload << ">";

  auto collection = _provider->connection();
  collection = expand_environment_variables(collection);

  auto dir_name = dbfs::mkdir(collection).append("/");
  auto collection_names = dbfs::find_subdirs(dir_name);

  for (auto const& collection_name : collection_names) {
    TLOG(16) << "FileSystemDB::configurationComposition() "
                "querying collection_name=<"
             << collection_name << ">";

    auto index_path = boost::filesystem::path(dir_name.c_str()).append(collection_name).append(dbfsl::search_index);

    SearchIndex search_index(index_path);

    auto configentityname_pairs = search_index.findAllGlobalConfigurations(query_payload);

    TLOG(16) << "FileSystemDB::configurationComposition() search "
                "returned "
             << configentityname_pairs.size() << " configurations.";

    for (auto const& configentityname_pair : configentityname_pairs) {
      std::ostringstream oss;

      oss << "{";
      oss << db::quoted_(apiliteral::option::collection) << ":" << db::quoted_(collection_name) << ",";
      oss << db::quoted_(apiliteral::option::provider) << ":" << db::quoted_(apiliteral::provider::filesystem) << ",";
      oss << db::quoted_(apiliteral::option::format) << ":" << db::quoted_(apiliteral::format::gui) << ",";
      oss << db::quoted_(apiliteral::option::operation) << ":" << db::quoted_(apiliteral::operation::readdocument) << ",";
      oss << db::quoted_(apiliteral::option::searchfilter) << ":"
          << "{";
      oss << db::quoted_(apiliteral::filter::configurations) << ":" << db::quoted_(configentityname_pair.first);
      oss << "," << db::quoted_(apiliteral::filter::entities) << ":" << db::quoted_(configentityname_pair.second);
      oss << "}";
      oss << "}";

      TLOG(16) << "FileSystemDB::configurationComposition() "
                  "found document=<"
               << oss.str() << ">";

      returnCollection.emplace_back(oss.str());
    }
  }

  return returnCollection;
}

template <>
template <>
std::vector<JSONDocument> StorageProvider<JSONDocument, FileSystemDB>::findVersions(JSONDocument const& filter) {
  confirm(!filter.empty());
  auto returnCollection = std::vector<JSONDocument>();

  TLOG(15) << "FileSystemDB::findVersions() begin";
  TLOG(15) << "FileSystemDB::findVersions() args data=<" << filter << ">";

  auto query_payload_document = JSONDocument{filter};

  auto collection_name = query_payload_document.findChild(apiliteral::option::collection).value();
  auto query_payload = query_payload_document.findChild(apiliteral::option::searchfilter).value();

  auto collection = _provider->connection() + collection_name;
  collection = expand_environment_variables(collection);

  TLOG(15) << "FileSystemDB::readDocument() collection_path=<" << collection << ">.";

  auto dir_name = dbfs::mkdir(collection).append("/");

  auto index_path = boost::filesystem::path(dir_name.c_str()).append(dbfsl::search_index);

  SearchIndex search_index(index_path);

  jsn::object_t search_ast;

  if (!jsn::JsonReader{}.read(query_payload, search_ast)) {
    TLOG(15) << "FileSystemDB::index::findVersionsByEntityName()"
             << " Failed to create an AST from search.";
    return returnCollection;
  }

  if (search_ast.count(apiliteral::filter::configurations) == 0) {
    auto versionentityname_pairs = search_index.findVersionsByEntityName(query_payload);

    TLOG(15) << "FileSystemDB::findVersionsByEntityName() search returned " << versionentityname_pairs.size() << " configurations.";

    for (auto const& versionentityname_pair : versionentityname_pairs) {
      std::ostringstream oss;

      oss << "{";
      oss << db::quoted_(apiliteral::option::collection) << ":" << db::quoted_(collection_name) << ",";
      oss << db::quoted_(apiliteral::option::provider) << ":" << db::quoted_(apiliteral::provider::filesystem) << ",";
      oss << db::quoted_(apiliteral::option::format) << ":" << db::quoted_(apiliteral::format::gui) << ",";
      oss << db::quoted_(apiliteral::option::operation) << ":" << db::quoted_(apiliteral::operation::readdocument) << ",";
      oss << db::quoted_(apiliteral::option::searchfilter) << ":"
          << "{";
      oss << db::quoted_(apiliteral::filter::version) << ":" << db::quoted_(versionentityname_pair.second);
      oss << "," << db::quoted_(apiliteral::filter::entities) << ":" << db::quoted_(versionentityname_pair.first);
      oss << "}";
      oss << "}";

      TLOG(15) << "FileSystemDB::findVersions() found document=<" << oss.str() << ">";

      returnCollection.emplace_back(oss.str());
    }
  } else {
    auto entityName = std::string{"notprovided"};

    try {
      entityName = boost::get<std::string>(search_ast.at(apiliteral::filter::entities));

      TLOG(15) << "FileSystemDB::findVersionsByGlobalConfigName"
               << " Found entity filter=<" << entityName << ">.";

    } catch (...) {
    }

    auto versionentityname_pairs = search_index.findVersionsByGlobalConfigName(query_payload);

    TLOG(15) << "FileSystemDB::findVersionsByGlobalConfigName() "
                "search returned "
             << versionentityname_pairs.size() << " configurations.";

    for (auto const& versionentityname_pair : versionentityname_pairs) {
      std::ostringstream oss;

      oss << "{";
      oss << db::quoted_(apiliteral::option::collection) << ":" << db::quoted_(collection_name) << ",";
      oss << db::quoted_(apiliteral::option::provider) << ":" << db::quoted_(apiliteral::provider::filesystem) << ",";
      oss << db::quoted_(apiliteral::option::format) << ":" << db::quoted_(apiliteral::format::gui) << ",";
      oss << db::quoted_(apiliteral::option::operation) << ":" << db::quoted_(apiliteral::operation::readdocument) << ",";
      oss << db::quoted_(apiliteral::option::searchfilter) << ":"
          << "{";
      oss << db::quoted_(apiliteral::filter::version) << ":" << db::quoted_(versionentityname_pair.second);
      oss << "," << db::quoted_(apiliteral::filter::entities) << ":" << db::quoted_(entityName);
      oss << "}";
      oss << "}";

      TLOG(15) << "FileSystemDB::findVersions() found document=<" << oss.str() << ">";

      returnCollection.emplace_back(oss.str());
    }
  }
  return returnCollection;
}

template <>
template <>
std::vector<JSONDocument> StorageProvider<JSONDocument, FileSystemDB>::findEntities(JSONDocument const& filter) {
  confirm(!filter.empty());
  auto returnCollection = std::vector<JSONDocument>();

  TLOG(19) << "FileSystemDB::findEntities() begin";
  TLOG(19) << "FileSystemDB::findEntities() args data=<" << filter << ">";

  auto query_payload_document = JSONDocument{filter};
  auto query_payload = query_payload_document.findChild(apiliteral::option::searchfilter).value();

  auto collection = _provider->connection();
  collection = expand_environment_variables(collection);

  auto dir_name = dbfs::mkdir(collection).append("/");
  auto collection_names = dbfs::find_subdirs(dir_name);

  for (auto const& collection_name : collection_names) {
    TLOG(19) << "FileSystemDB::findEntities() querying "
                "collection_name=<"
             << collection_name << ">";

    auto index_path = boost::filesystem::path(dir_name.c_str()).append(collection_name).append(dbfsl::search_index);

    SearchIndex search_index(index_path);

    auto configentity_names = search_index.findEntities(query_payload);

    if (configentity_names.size() > 1) {
      std::sort(configentity_names.begin(), configentity_names.end());
      auto unique_configentity_names = std::vector<std::string>{};

      std::unique_copy(configentity_names.begin(), configentity_names.end(), back_inserter(unique_configentity_names));
      configentity_names.swap(unique_configentity_names);
    }

    TLOG(19) << "FileSystemDB::findEntities() search returned " << configentity_names.size() << " configurations.";

    for (auto const& configentity_name : configentity_names) {
      std::ostringstream oss;

      oss << "{";
      oss << db::quoted_(apiliteral::option::collection) << ":" << db::quoted_(collection_name) << ",";
      oss << db::quoted_(apiliteral::option::provider) << ":" << db::quoted_(apiliteral::provider::filesystem) << ",";
      oss << db::quoted_(apiliteral::option::format) << ":" << db::quoted_(apiliteral::format::gui) << ",";
      oss << db::quoted_(apiliteral::option::operation) << ":" << db::quoted_(apiliteral::operation::findversions) << ",";
      oss << db::quoted_(apiliteral::option::searchfilter) << ":"
          << "{";
      oss << db::quoted_(apiliteral::filter::entities) << ":" << db::quoted_(configentity_name);
      oss << "}";
      oss << "}";

      TLOG(19) << "FileSystemDB::findEntities() found document=<" << oss.str() << ">";

      returnCollection.emplace_back(oss.str());
    }
  }

  return returnCollection;
}

template <>
template <>
std::vector<JSONDocument> StorageProvider<JSONDocument, FileSystemDB>::listCollections(JSONDocument const& query_payload) {
  confirm(!query_payload.empty());
  auto returnCollection = std::vector<JSONDocument>();
  TLOG(22) << "FileSystemDB::listCollections() begin";
  TLOG(22) << "FileSystemDB::listCollections() args data=<" << query_payload << ">";

  auto collection = _provider->connection();
  collection = expand_environment_variables(collection);

  auto dir_name = dbfs::mkdir(collection).append("/");
  auto collection_names = dbfs::find_subdirs(dir_name);

  for (auto const& collection_name : collection_names) {
    if (collection_name == "system.indexes" || collection_name == system_metadata) {
      continue;
    }

    TLOG(22) << "FileSystemDB::listCollections() found "
                "collection_name=<"
             << collection_name << ">";

    std::ostringstream oss;

    oss << "{";
    oss << db::quoted_(apiliteral::option::collection) << ":" << db::quoted_(collection_name) << ",";
    oss << db::quoted_(apiliteral::option::provider) << ":" << db::quoted_(apiliteral::provider::filesystem) << ",";
    oss << db::quoted_(apiliteral::option::format) << ":" << db::quoted_(apiliteral::format::gui) << ",";
    oss << db::quoted_(apiliteral::option::operation) << ":" << db::quoted_(apiliteral::operation::findversions) << ",";
    oss << db::quoted_(apiliteral::option::searchfilter) << ":" << apiliteral::empty_json;
    oss << "}";

    TLOG(22) << "FileSystemDB::listCollections() found document=<" << oss.str() << ">";

    returnCollection.emplace_back(oss.str());
  }
  return returnCollection;
}

template <>
template <>
std::vector<JSONDocument> StorageProvider<JSONDocument, FileSystemDB>::addConfiguration(JSONDocument const& query_payload) {
  confirm(!query_payload.empty());
  auto returnCollection = std::vector<JSONDocument>();

  TLOG(15) << "FileSystemDB::addConfiguration() begin";
  TLOG(15) << "FileSystemDB::addConfiguration() args data=<" << query_payload << ">";

  return returnCollection;
}

template <>
template <>
std::vector<JSONDocument> StorageProvider<JSONDocument, FileSystemDB>::listDatabases(JSONDocument const& query_payload) {
  confirm(!query_payload.empty());
  auto returnCollection = std::vector<JSONDocument>();

  TLOG(19) << "FileSystemDB::listDatabases() begin";
  TLOG(19) << "FileSystemDB::listDatabases() args data=<" << query_payload << ">";

  auto database = _provider->connection();

  auto found = database.find_last_not_of("\\/");
  if (found != std::string::npos) {
    database.erase(found + 1);
  }

  auto database_names = dbfs::find_siblingdirs(database);

  for (auto const& database_name : database_names) {
    TLOG(19) << "FileSystemDB::listDatabases() found databases=<" << database_name << ">";

    if (database_name == "system") {
      continue;
    }

    TLOG(19) << "FileSystemDB::listDatabases() found database_name=<" << database_name << ">";

    std::ostringstream oss;
    oss << "{";
    oss << db::quoted_(apiliteral::database) << ":" << db::quoted_(database_name) << ",";
    oss << db::quoted_(apiliteral::option::provider) << ":" << db::quoted_(apiliteral::provider::filesystem) << ",";
    oss << db::quoted_(apiliteral::option::format) << ":" << db::quoted_(apiliteral::format::gui) << ",";
    oss << db::quoted_(apiliteral::option::searchfilter) << ":" << apiliteral::empty_json;
    oss << "}";

    TLOG(19) << "FileSystemDB::listDatabases() found document=<" << oss.str() << ">";

    returnCollection.emplace_back(oss.str());
  }

  return returnCollection;
}

template <>
template <>
std::vector<JSONDocument> StorageProvider<JSONDocument, FileSystemDB>::databaseMetadata(JSONDocument const& query_payload [[gnu::unused]]) {
  confirm(!query_payload.empty());
  auto returnCollection = std::vector<JSONDocument>();

  auto collection = _provider->connection() + system_metadata;
  collection = expand_environment_variables(collection);

  if (collection.find(dbfsl::FILEURI) == 0) {
    collection = collection.substr(strlen(dbfsl::FILEURI));
  }

  TLOG(13) << "FileSystemDB::databaseMetadata() collection_path=<" << collection << ">.";

  auto oids = dbfs::find_documents(collection);

  TLOG(13) << "FileSystemDB::databaseMetadata() search returned " << oids.size() << " documents.";

  for (auto const& oid : oids) {
    auto doc_path = boost::filesystem::path(collection).append(oid).replace_extension(".json");

    TLOG(13) << "FileSystemDB::databaseMetadata() reading document <" << doc_path.c_str() << ">.";

    auto json = std::string{};

    db::read_buffer_from_file(json, {doc_path.c_str()});

    TLOG(13) << "FileSystemDB::databaseMetadata() document <" << json << ">.";

    returnCollection.emplace_back(json);
  }

  return returnCollection;
}

template <>
template <>
std::vector<JSONDocument> StorageProvider<JSONDocument, FileSystemDB>::searchCollection(JSONDocument const& query_payload) {
  confirm(!query_payload.empty());
  auto returnCollection = std::vector<JSONDocument>();

  TLOG(15) << "StorageProvider::FileSystemDB::searchCollection() begin";
  TLOG(15) << "StorageProvider::FileSystemDB::searchCollection() args data=<" << query_payload << ">";

  throw runtime_error("FileSystemDB") << "StorageProvider::FileSystemDB::searchCollection() is not implemented";

  return returnCollection;
}

namespace filesystem {
namespace debug {
void enable() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TLOG(10) << "artdaq::database::filesystem trace_enable";
  artdaq::database::filesystem::debug::ReadWrite();
  artdaq::database::filesystem::index::debug::enable();
}
}  // namespace debug
}  // namespace filesystem

}  // namespace database
}  // namespace artdaq
