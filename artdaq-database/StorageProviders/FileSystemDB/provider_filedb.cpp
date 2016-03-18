#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h"
#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb_index.h"
#include "artdaq-database/StorageProviders/common.h"

#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"

#include <wordexp.h>
#include <boost/filesystem.hpp>
#include <boost/range/adaptors.hpp>

#include "artdaq-database/BasicTypes/basictypes.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "PRVDR:FileDB_C"

namespace dbfsl = artdaq::database::filesystem::literal;

namespace artdaq {
namespace database {

using artdaq::database::basictypes::JsonData;
using artdaq::database::filesystem::FileSystemDB;
using artdaq::database::filesystem::index::SearchIndex;

using artdaq::database::jsonutils::JSONDocumentBuilder;
using artdaq::database::jsonutils::JSONDocument;

std::string mkdir(std::string const&);
object_id_t generate_oid();
object_id_t extract_oid(std::string const&);
std::list<std::string> find_subdirs(std::string const&);

std::string expand_environment_variables(std::string var);

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, FileSystemDB>::load(JsonData const& filter) {
  TRACE_(3, "StorageProvider::FileSystemDB::load() begin");
  TRACE_(3, "StorageProvider::FileSystemDB::load() args search=<" << filter.json_buffer << ">");

  auto builder = JSONDocumentBuilder{filter.json_buffer};

  auto collection_name = builder.extract().findChild("collection").value();
  auto search_filter = builder.extract().findChild("filter").value();

  auto collection = _provider->connection() + collection_name;
  collection = expand_environment_variables(collection);

  TRACE_(3, "StorageProvider::FileSystemDB::load() collection_path=<" << collection << ">.");

  auto dir_name = mkdir(collection);

  auto index_path = boost::filesystem::path(dir_name.c_str()).append(dbfsl::search_index);

  SearchIndex search_index(index_path);

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

  auto builder = JSONDocumentBuilder{data.json_buffer};

  auto oid = generate_oid();
  TRACE_(4, "StorageProvider::FileSystemDB::store() oid=<" << oid << ">.");

  std::stringstream ss;

  ss << "{ \"_id\":\"" << oid << "\"}";

  // builder.setObjectID( {ss.str()} );

  TRACE_(4, "StorageProvider::FileSystemDB::store() document=<" << builder.extract() << ">.");

  auto collection_name = builder.extract().findChild("collection").value();

  try {
    auto filter = builder.extract().findChild("filter").value();
    TRACE_(4, "StorageProvider::FileSystemDB::store() found filter=<" << filter << ">.");
    oid = extract_oid(filter);
    TRACE_(4, "StorageProvider::FileSystemDB::store() using provided oid=<" << oid << ">.");
  } catch (...) {
  }

  auto collection = _provider->connection() + collection_name;

  collection = expand_environment_variables(collection);

  TRACE_(4, "StorageProvider::FileSystemDB::store() collection_path=<" << collection << ">.");

  auto filename = mkdir(collection) + oid + ".json";

  TRACE_(4, "StorageProvider::FileSystemDB::store() filename=<" << filename << ">.");

  std::ofstream os(filename);

  auto json = builder.extract().findChild("document").value();

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
  assert(!search_filter.json_buffer.empty());
  auto returnCollection = std::list<JsonData>();

  TRACE_(5, "StorageProvider::FileSystemDB::findGlobalConfigs() begin");
  TRACE_(5, "StorageProvider::FileSystemDB::findGlobalConfigs() args data=<" << search_filter.json_buffer << ">");

  auto collection = _provider->connection();
  collection = expand_environment_variables(collection);

  auto dir_name = mkdir(collection);
  auto collection_names = find_subdirs(dir_name);

  for (auto const& collection_name : collection_names) {
    TRACE_(5, "StorageProvider::FileSystemDB::findGlobalConfigs() querying collection_name=<" << collection_name
                                                                                              << ">");

    auto index_path = boost::filesystem::path(dir_name.c_str()).append(collection_name).append(dbfsl::search_index);

    SearchIndex search_index(index_path);

    auto configentityname_pairs = search_index.findAllGlobalConfigurations(search_filter);

    TRACE_(5, "StorageProvider::FileSystemDB::findGlobalConfigs() search returned " << configentityname_pairs.size()
                                                                                    << " configurations.");

    for (auto const& configentityname_pair : configentityname_pairs) {
      std::stringstream ss;

      ss << "{";
      // ss << "\"collection\" : \"" << collection_name << "\",";
      ss << "\"dbprovider\" : \"filesystem\",";
      ss << "\"dataformat\" : \"gui\",";
      ss << "\"operation\" : \"buildfilter\",";
      ss << "\"filter\" : {";
      ss << "\"configurations.name\" : \"" << configentityname_pair.first << "\"";
      ss << "}";
      ss << "}";

      TRACE_(5, "StorageProvider::FileSystemDB::findGlobalConfigs() found document=<" << ss.str() << ">");

      returnCollection.emplace_back(ss.str());
    }
  }

  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, FileSystemDB>::buildConfigSearchFilter(JsonData const& search_filter) {
  assert(!search_filter.json_buffer.empty());
  auto returnCollection = std::list<JsonData>();

  TRACE_(6, "StorageProvider::FileSystemDB::buildConfigSearchFilter() begin");
  TRACE_(6, "StorageProvider::FileSystemDB::buildConfigSearchFilter() args data=<" << search_filter.json_buffer << ">");

  auto collection = _provider->connection();
  collection = expand_environment_variables(collection);

  auto dir_name = mkdir(collection);
  auto collection_names = find_subdirs(dir_name);

  for (auto const& collection_name : collection_names) {
    TRACE_(6, "StorageProvider::FileSystemDB::buildConfigSearchFilter() querying collection_name=<" << collection_name
                                                                                                    << ">");

    auto index_path = boost::filesystem::path(dir_name.c_str()).append(collection_name).append(dbfsl::search_index);

    SearchIndex search_index(index_path);

    auto configentityname_pairs = search_index.findAllGlobalConfigurations(search_filter);

    TRACE_(6, "StorageProvider::FileSystemDB::buildConfigSearchFilter() search returned "
                  << configentityname_pairs.size() << " configurations.");

    for (auto const& configentityname_pair : configentityname_pairs) {
      std::stringstream ss;

      ss << "{";
      ss << "\"collection\" : \"" << collection_name << "\",";
      ss << "\"dbprovider\" : \"filesystem\",";
      ss << "\"dataformat\" : \"gui\",";
      ss << "\"operation\" : \"load\",";
      ss << "\"filter\" : {";
      ss << "\"configurations.name\" : \"" << configentityname_pair.first << "\"";
      ss << ", \"configurable_entity.name\" : \"" << configentityname_pair.second << "\"";
      ss << "}";
      ss << "}";

      TRACE_(6, "StorageProvider::FileSystemDB::buildConfigSearchFilter() found document=<" << ss.str() << ">");

      returnCollection.emplace_back(ss.str());
    }
  }

  return returnCollection;
}

std::string mkdir(std::string const& d) {
  assert(!d.empty());

  auto dir = d;

  if (dir.find(dbfsl::FILEURI) == 0) dir = dir.substr(strlen(dbfsl::FILEURI));

  TRACE_(11, "StorageProvider::FileSystemDB mkdir dir=<" << dir << ">");

  auto path = boost::filesystem::path(dir.c_str());

  if (boost::filesystem::exists(path)) {
    if (boost::filesystem::is_directory(path)) {
      TRACE_(11, "StorageProvider::FileSystemDB Directory exists, checking permissions; path=<" << dir << ">");

      auto mask = boost::filesystem::perms::owner_write | boost::filesystem::perms::owner_read;

      if ((boost::filesystem::status(path).permissions() & mask) != mask) {
        TRACE_(11, "StorageProvider::FileSystemDB Directory  <"
                       << dir << "> has wrong permissions; needs to be owner readable and writable");
        throw cet::exception("FileSystemDB") << "Directory  <" << dir
                                             << "> has wrong permissions; needs to be owner readable and writable";
      }
    } else {
      TRACE_(11, "StorageProvider::FileSystemDB Failed creating a directory, sometging in the way path=<" << dir
                                                                                                          << ">");
      throw cet::exception("FileSystemDB") << "Failed creating a directory, sometging in the way path=<" << dir << ">";
    }
  }

  boost::system::error_code ec;

  if (!boost::filesystem::create_directories(path, ec)) {
    if (ec != boost::system::errc::success) {
      TRACE_(11, "StorageProvider::FileSystemDB Failed creating a directory path=<" << dir
                                                                                    << "> error code=" << ec.message());
      throw cet::exception("FileSystemDB") << "Failed creating a directory path=<" << dir
                                           << "> error code=" << ec.message();
    }
  }

  auto perms = boost::filesystem::perms::add_perms | boost::filesystem::perms::owner_write |
               boost::filesystem::perms::owner_read;

  ec.clear();

  boost::filesystem::permissions(path, perms, ec);

  if (ec != boost::system::errc::success) {
    TRACE_(11, "StorageProvider::FileSystemDB Failed enforcing directory permissions for path=<"
                   << dir << "> error code=" << ec.message());
    throw cet::exception("FileSystemDB") << "Failed enforcing directory permissions for path=<" << dir
                                         << "> error code=" << ec.message();
  }

  return dir;
}

std::list<std::string> find_subdirs(std::string const& d) {
  assert(!d.empty());

  auto returnValue = std::list<std::string>{};

  auto dir = d;

  if (dir.find(dbfsl::FILEURI) == 0) dir = dir.substr(strlen(dbfsl::FILEURI));

  TRACE_(11, "StorageProvider::FileSystemDB find_subdirs dir=<" << dir << ">");

  auto path = boost::filesystem::path(dir.c_str());

  if (!boost::filesystem::exists(path)) {
    TRACE_(11, "StorageProvider::FileSystemDB Failed searching for subdirectories, directory does not exist path=<"
                   << dir << ">");
    throw cet::exception("FileSystemDB") << "Failed searching for subdirectories, directory does not exist path=<"
                                         << dir << ">";
  }

  if (!boost::filesystem::is_directory(path)) {
    TRACE_(11, "StorageProvider::FileSystemDB Failed searching for subdirectories, not a directory path=<" << dir
                                                                                                           << ">");
    throw cet::exception("FileSystemDB") << "Failed searching for subdirectories, not a directory path=<" << dir << ">";
  }

  boost::filesystem::directory_iterator end_iter;

  for (boost::filesystem::directory_iterator dir_iter(path); dir_iter != end_iter; ++dir_iter) {
    if (!boost::filesystem::is_directory(dir_iter->status())) continue;    
    TRACE_(11, "StorageProvider::FileSystemDB Found subdirectory, path=<" << dir_iter->path().filename().string() << ">");

    returnValue.emplace_back(dir_iter->path().filename().string());
  }

  return returnValue;
}

object_id_t generate_oid() {
  std::ifstream is("/proc/sys/kernel/random/uuid");

  std::string oid((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());

  oid.erase(std::remove(oid.begin(), oid.end(), '-'), oid.end());
  oid.resize(24);

  TRACE_(12, "StorageProvider::FileSystemDB generate_oid=" << oid);

  return oid;
}

std::string expand_environment_variables(std::string var) {
  wordexp_t p;
  char** w;

  ::wordexp(var.c_str(), &p, 0);

  w = p.we_wordv;

  std::stringstream ss;

  for (size_t i = 0; i < p.we_wordc; i++) ss << w[i] << "/";

  ::wordfree(&p);
  
  return ss.str();
 // auto retValue = ss.str();  
 // return retValue.substr(0, retValue.find_last_of("\\/"));;
}

object_id_t extract_oid(std::string const& filter) {
  auto ex = std::regex("^\\{[^:]+:\\s+([\\s\\S]+)\\}$");

  auto results = std::smatch();

  if (!std::regex_search(filter, results, ex))
    throw cet::exception("JSONDocument") << ("Regex search failed; JSON buffer:" + filter);

  if (results.size() != 2) {
    // we are interested in a second match
    TRACE_(12, "value()"
                   << "JSON regex_search() result count=" << results.size());
    for (auto const& result : results) {
      TRACE_(12, "value()"
                     << "JSON regex_search() result=" << result);
    }

    throw cet::exception("JSONDocument") << ("Regex search failed, regex_search().size()!=1; JSON buffer: " + filter);
  }

  auto match = std::string(results[1]);

  match.erase(match.find_last_not_of(" \n\r\t") + 1);

  auto dequote = [](auto s) {

    if (s[0] == '"' && s[s.length() - 1] == '"')
      return s.substr(1, s.length() - 2);
    else
      return s;
  };

  match = dequote(match);

  TRACE_(12, "value()"
                 << "JSON regex_search() result=" << match);

  return match;
}

namespace filesystem {
void trace_enable() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", 1LL);
  TRACE_CNTL("modeS", 1LL);

  TRACE_(0, "artdaq::database::filesystem::"
                << "trace_enable");

  index::trace_enable();
}
}  // namespace filesystem
}  // namespace database
}  // namespace artdaq
