#include "artdaq-database/SharedCommon/sharedcommon_common.h"
#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h"
#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb_index.h"
#include "artdaq-database/StorageProviders/common.h"

#include <fstream>
#include "artdaq-database/BasicTypes/data_json.h"
#include "artdaq-database/DataFormats/Json/json_common.h"

#include <boost/filesystem.hpp>

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "PRVDR:FileDBIX_C"

using namespace artdaq::database;
using namespace artdaq::database::filesystem::index;

namespace db = artdaq::database;
namespace dbfs = db::filesystem;
namespace dbfsi = dbfs::index;
namespace jsn = db::json;

using artdaq::database::json::JsonReader;
using artdaq::database::json::JsonWriter;
using artdaq::database::sharedtypes::unwrap;

typedef std::vector<object_id_t> (SearchIndex::*matching_function_t)(std::string const&) const;

SearchIndex::SearchIndex(boost::filesystem::path const& path)
    : _index{}, _path{path}, _isDirty{false}, _isOpen{_open(path)} {}

SearchIndex::~SearchIndex() {
  try {
    _close();
  } catch (...) {
    TRACE_(10, "Exception in StorageProvider::FileSystemDB::~SearchIndex() "
                   << ::debug::current_exception_diagnostic_information());
  }
}

std::vector<object_id_t> SearchIndex::findDocumentIDs(JsonData const& search) {
  auto ouids = std::vector<object_id_t>{};

  confirm(!search.empty());

  TRACE_(2, "StorageProvider::FileSystemDB::index::findDocumentIDs() begin");
  TRACE_(2, "StorageProvider::FileSystemDB::index::findDocumentIDs() args search=<" << search << ">.");

  auto reader = JsonReader{};

  object_t search_ast;

  if (!reader.read(search, search_ast)) {
    TRACE_(5,
           "StorageProvider::FileSystemDB::index::findDocumentIDs() Failed "
           "to create an AST from search.");
    return ouids;
  }

  TRACE_(5, "StorageProvider::FileSystemDB::index::findDocumentIDs() found " << search_ast.size()
                                                                             << " search criteria.");

  if (search_ast.empty()) {
    ouids = SearchIndex::getObjectIds();

    TRACE_(5, "StorageProvider::FileSystemDB::index::findDocumentIDs() found " << ouids.size() << " documents.");

    return ouids;
  }

  auto runMatchingFunction = [](std::string const& name) {
    auto matching_function_map =
        std::map<std::string, matching_function_t>{{"version", &SearchIndex::_matchVersion},
                                                   {"configurations.name", &SearchIndex::_matchConfiguration},
                                                   {"entities.name", &SearchIndex::_matchEntity},
                                                   {"runs.name", &SearchIndex::_matchRun},
                                                   {"aliases.active.name", &SearchIndex::_matchVersionAlias},
                                                   {"$oid", &SearchIndex::_matchObjectId},
                                                   {"_id", &SearchIndex::_matchObjectIds}};

    return matching_function_map.at(name);
  };

  ouids.reserve(128);

  auto criteria_count = int{0};

  for (auto const& search_criterion : search_ast) {
    if (search_criterion.value.type() == typeid(std::string)) {
      auto const& value = boost::get<std::string>(search_criterion.value);
      auto matches = (this->*runMatchingFunction(search_criterion.key))(value);
      ++criteria_count;
      ouids.insert(ouids.end(), matches.begin(), matches.end());
    } else if (search_criterion.value.type() == typeid(jsn::object_t)) {
      auto const& value = boost::get<jsn::object_t>(search_criterion.value);
      auto writer = JsonWriter{};
      auto json = std::string();
      if (!writer.write(value, json)) {
        TRACE_(5,
               "StorageProvider::FileSystemDB::index::findDocumentIDs() "
               "Failed to write an AST to json, key=<"
                   << search_criterion.key << ">.");
        throw runtime_error("FileSystemDB") << "StorageProvider::FileSystemDB::index::findDocumentIDs() Failed "
                                               "to write an AST to json.";
      }

      TRACE_(5,
             "StorageProvider::FileSystemDB::index::findDocumentIDs() search_criterion.value.type() == "
             "typeid(jsn::object_t),"
                 << " key=<" << search_criterion.key << ">");

      auto matches = (this->*runMatchingFunction(search_criterion.key))(json);

      ouids.insert(ouids.end(), matches.begin(), matches.end());
    }
  }

  if (ouids.size() < 2) return ouids;

  if (criteria_count == 1) {
    std::sort(ouids.begin(), ouids.end());
    auto unique_ouids = std::vector<object_id_t>{};
    std::unique_copy(ouids.begin(), ouids.end(), back_inserter(unique_ouids));
    ouids.swap(unique_ouids);

    TRACE_(5,
           "StorageProvider::FileSystemDB::index::findDocumentIDs() "
           "criteria_count=1 found "
               << ouids.size() << " documents.");

  } else {
    auto groupby_map = std::map<object_id_t, int>{};
    auto max_count = int{0};

    std::for_each(ouids.begin(), ouids.end(), [&groupby_map, &max_count](object_id_t const& ouid) {
      groupby_map[ouid]++;
      max_count = std::max(max_count, groupby_map[ouid]);
    });

    auto returnValue = std::vector<object_id_t>{};

    if (max_count < criteria_count)  // no matches return empty
      ouids.swap(returnValue);

    std::for_each(groupby_map.begin(), groupby_map.end(), [&criteria_count, &returnValue](auto const& groupby_element) {
      if (groupby_element.second >= criteria_count) returnValue.push_back(groupby_element.first);
    });

    std::sort(returnValue.begin(), returnValue.end());
    auto unique_ouids = std::vector<object_id_t>{};
    std::unique_copy(returnValue.begin(), returnValue.end(), back_inserter(unique_ouids));
    ouids.swap(unique_ouids);

    TRACE_(5,
           "StorageProvider::FileSystemDB::index::findDocumentIDs() "
           "criteria_count="
               << criteria_count << ", max_count=" << max_count << ", found " << ouids.size() << " documents.");
  }

  TRACE_(5, "StorageProvider::FileSystemDB::index::findDocumentIDs() returning " << ouids.size() << " documents.");

  /*
  std::cout << "StorageProvider::FileSystemDB::index::findDocumentIDs() \n";

  for(auto const& ouid: ouids){
    std::cout <<  ouid << "\n";
  }
  */
  return ouids;
}

std::vector<std::pair<std::string, std::string>> SearchIndex::findVersionsByGlobalConfigName(JsonData const& search) {
  confirm(!search.empty());
  auto returnCollection = std::vector<std::pair<std::string, std::string>>{};
  TRACE_(5,
         "StorageProvider::FileSystemDB::index::"
         "findVersionsByGlobalConfigName() begin");
  TRACE_(5,
         "StorageProvider::FileSystemDB::index::"
         "findVersionsByGlobalConfigName() args search=<"
             << search << ">.");

  auto reader = JsonReader{};

  object_t search_ast;

  if (!reader.read(search, search_ast)) {
    TRACE_(5, "StorageProvider::FileSystemDB::index::findVersionsByGlobalConfigName()"
                  << " Failed to create an AST from search.");
    return returnCollection;
  }

  auto configNameFilter = std::string{};

  try {
    configNameFilter = boost::get<std::string>(search_ast.at("configurations.name"));

    TRACE_(5, "StorageProvider::FileSystemDB::index::findVersionsByGlobalConfigName()"
                  << " Found global configuration filter=<" << configNameFilter << ">.");

  } catch (...) {
  }

  return _indexed_filtered_innerjoin_over_ouid("version", "configurations.name", configNameFilter);
}

std::vector<std::pair<std::string, std::string>> SearchIndex::findVersionsByEntityName(JsonData const& search) {
  confirm(!search.empty());
  auto returnCollection = std::vector<std::pair<std::string, std::string>>{};
  TRACE_(5, "StorageProvider::FileSystemDB::index::findVersionsByEntityName() begin");
  TRACE_(5, "StorageProvider::FileSystemDB::index::findVersionsByEntityName() args search=<" << search << ">.");

  auto reader = JsonReader{};

  object_t search_ast;

  if (!reader.read(search, search_ast)) {
    TRACE_(5, "StorageProvider::FileSystemDB::index::findVersionsByEntityName()"
                  << " Failed to create an AST from search.");
    return returnCollection;
  }

  auto entityNameFilter = std::string{"notprovided"};

  try {
    entityNameFilter = boost::get<std::string>(search_ast.at("entities.name"));

    TRACE_(5, "StorageProvider::FileSystemDB::index::findVersionsByEntityName()"
                  << " Found entity filter=<" << entityNameFilter << ">.");

  } catch (...) {
  }

  return _indexed_filtered_innerjoin_over_ouid("version", "entities.name", entityNameFilter);
}

std::vector<std::string> SearchIndex::findEntities(JsonData const& search) {
  confirm(!search.empty());
  auto returnCollection = std::vector<std::string>{};
  TRACE_(5, "StorageProvider::FileSystemDB::index::findEntities() begin");
  TRACE_(5, "StorageProvider::FileSystemDB::index::findEntities() args search=<" << search << ">.");

  auto reader = JsonReader{};

  object_t search_ast;

  if (!reader.read(search, search_ast)) {
    TRACE_(5, "StorageProvider::FileSystemDB::index::findEntities()"
                  << " Failed to create an AST from search.");
    return returnCollection;
  }

  auto entityNameFilter = std::string{};

  try {
    entityNameFilter = boost::get<std::string>(search_ast.at("entities.name"));

    TRACE_(5, "StorageProvider::FileSystemDB::index::findEntities()"
                  << " Found filter=<" << entityNameFilter << ">.");

  } catch (...) {
  }

  return _filtered_attribute_list("entities.name", entityNameFilter);
}

std::vector<std::pair<std::string, std::string>> SearchIndex::findAllGlobalConfigurations(JsonData const& search) {
  confirm(!search.empty());
  auto returnCollection = std::vector<std::pair<std::string, std::string>>{};
  TRACE_(5,
         "StorageProvider::FileSystemDB::index::findAllGlobalConfigurations("
         ") begin");
  TRACE_(5,
         "StorageProvider::FileSystemDB::index::findAllGlobalConfigurations("
         ") args search=<"
             << search << ">.");

  auto reader = JsonReader{};

  object_t search_ast;

  if (!reader.read(search, search_ast)) {
    TRACE_(5, "StorageProvider::FileSystemDB::index::findAllGlobalConfigurations()"
                  << " Failed to create an AST from search.");
    return returnCollection;
  }

  auto configFilter = std::string{};

  try {
    configFilter = boost::get<std::string>(search_ast.at("configurations.name"));
    TRACE_(5, "StorageProvider::FileSystemDB::index::findAllGlobalConfigurations()"
                  << " Found filter=<" << configFilter << ">.");
  } catch (...) {
  }

  return _indexed_filtered_innerjoin_over_ouid("entities.name", "configurations.name", configFilter);
}

bool SearchIndex::addDocument(JsonData const& document, object_id_t const& ouid) {
  confirm(!document.empty());
  confirm(!ouid.empty());

  TRACE_(5, "StorageProvider::FileSystemDB::index::addDocument() begin");
  TRACE_(5, "StorageProvider::FileSystemDB::index::addDocument() args document=<" << document << ">.");
  TRACE_(5, "StorageProvider::FileSystemDB::index::addDocument() args ouid=<" << ouid << ">.");

  auto reader = JsonReader{};

  object_t doc_ast;

  if (!reader.read(document, doc_ast)) {
    TRACE_(5,
           "StorageProvider::FileSystemDB::index::addDocument() Failed to "
           "create an AST from document.");
    return false;
  }

  TRACE_(5, "StorageProvider::FileSystemDB::index::addDocument() found " << doc_ast.size() << " keys.");

  try {
    _isDirty = true;

    _addId(ouid);

    auto version = boost::get<std::string>(doc_ast.at("version"));

    _addVersion(ouid, version);

    auto configs = boost::get<jsn::array_t>(doc_ast.at("configurations"));

    for (auto const& config : configs) {
      auto configuration = boost::get<std::string>(boost::get<jsn::object_t>(config).at("name"));
      _addConfiguration(ouid, configuration);
    }

    try {
      auto entities = boost::get<jsn::array_t>(doc_ast.at("entities"));

      for (auto const& entity : entities) {
        auto entity_name = boost::get<std::string>(boost::get<jsn::object_t>(entity).at("name"));
        _addEntity(ouid, entity_name);
      }
    } catch (...) {
      TRACE_(5,
             "StorageProvider::FileSystemDB::index::addDocument() Failed to "
             "add entities.");
    }

    try {
      auto aliasesnode = boost::get<jsn::object_t>(doc_ast.at("aliases"));
      auto aliases = boost::get<jsn::array_t>(aliasesnode.at("active"));
      for (auto const& alias : aliases) {
        auto alias_name = boost::get<std::string>(boost::get<jsn::object_t>(alias).at("name"));
        _addVersionAlias(ouid, alias_name);
      }
    } catch (...) {
      TRACE_(5,
             "StorageProvider::FileSystemDB::index::addDocument() Failed to "
             "add aliases.");
    }

    try {
      auto runs = boost::get<jsn::array_t>(doc_ast.at("runs"));
      for (auto const& run : runs) {
        auto run_name = boost::get<std::string>(boost::get<jsn::object_t>(run).at("name"));
        _addRun(ouid, run_name);
      }
    } catch (...) {
      TRACE_(5,
             "StorageProvider::FileSystemDB::index::addDocument() Failed to "
             "add run.");
    }

    return true;

  } catch (...) {
    for (auto const& kvp : doc_ast) {
      TRACE_(5, "StorageProvider::FileSystemDB::index::addDocument() key=<" << kvp.key << ">");
    }

    TRACE_(5, "Exception in StorageProvider::FileSystemDB::index::addDocument() "
                  << ::debug::current_exception_diagnostic_information());
  }

  return false;
}

bool SearchIndex::removeDocument(JsonData const& document, object_id_t const& ouid) {
  confirm(!document.empty());
  confirm(!ouid.empty());

  TRACE_(5, "StorageProvider::FileSystemDB::index::removeDocument() begin");
  TRACE_(5, "StorageProvider::FileSystemDB::index::removeDocument() args document=<" << document << ">.");
  TRACE_(5, "StorageProvider::FileSystemDB::index::removeDocument() args ouid=<" << ouid << ">.");

  auto reader = JsonReader{};

  object_t doc_ast;

  if (!reader.read(document, doc_ast)) {
    TRACE_(6,
           "StorageProvider::FileSystemDB::index::removeDocument() Failed "
           "to create an AST from document.");
    return false;
  }

  TRACE_(5, "StorageProvider::FileSystemDB::index::removeDocument() found " << doc_ast.size() << " keys.");

  try {
    _isDirty = true;

    _removeId(ouid);

    auto version = boost::get<std::string>(doc_ast.at("version"));

    _removeVersion(ouid, version);

    auto configs = boost::get<jsn::array_t>(doc_ast.at("configurations"));

    for (auto const& config : configs) {
      auto configuration = boost::get<std::string>(boost::get<jsn::object_t>(config).at("name"));
      _removeConfiguration(ouid, configuration);
    }

    auto aliasesnode = boost::get<jsn::object_t>(doc_ast.at("aliases"));
    auto aliases = boost::get<jsn::array_t>(aliasesnode.at("active"));
    for (auto const& alias : aliases) {
      auto alias_name = boost::get<std::string>(boost::get<jsn::object_t>(alias).at("name"));
      _removeConfiguration(ouid, alias_name);
    }

    auto runs = boost::get<jsn::array_t>(doc_ast.at("runs"));
    for (auto const& run : runs) {
      auto run_name = boost::get<std::string>(boost::get<jsn::object_t>(run).at("name"));
      _removeRun(ouid, run_name);
    }
    return true;

  } catch (...) {
    for (auto const& kvp : doc_ast) {
      TRACE_(5, "StorageProvider::FileSystemDB::index::removeDocument() key=<" << kvp.key << ">");
    }

    TRACE_(6, "Exception in StorageProvider::FileSystemDB::index::removeDocument() "
                  << ::debug::current_exception_diagnostic_information());
  }

  return false;
}

bool SearchIndex::_create(boost::filesystem::path const& index_path) {
  try {
    auto json = std::string{
        "{\"ouid\":[], \"version\":{},\"configurations.name\":{}, \"entities.name\":{}, \"aliases.active.name\":{}, "
        "\"runs.name\":{} }"};

    return db::write_buffer_to_file(json, {index_path.c_str()});
  } catch (...) {
    TRACE_(11, "Exception in StorageProvider::FileSystemDB::SearchIndex::_create() "
                   << ::debug::current_exception_diagnostic_information());
    return false;
  }
}

bool SearchIndex::_open(boost::filesystem::path const& index_path) {
  TRACE_(3, "StorageProvider::FileSystemDB::index::_open() begin");
  TRACE_(3, "StorageProvider::FileSystemDB::index::_open() args index_path=<" << index_path.c_str() << ">.");

  if (!_index.empty()) {
    TRACE_(3,
           "StorageProvider::FileSystemDB::SearchIndex SearchIndex was "
           "aready opened, path=<"
               << index_path.c_str() << ">");
    throw runtime_error("FileSystemDB") << "StorageProvider::FileSystemDB::SearchIndex SearchIndex was aready "
                                           "opened, path=<"
                                        << index_path.c_str() << ">";
  }

  if (!boost::filesystem::exists(boost::filesystem::status(index_path)) && !_create(index_path)) {
    TRACE_(3,
           "StorageProvider::FileSystemDB::SearchIndex Failed creating "
           "SearchIndex, path=<"
               << index_path.c_str() << ">");
    throw runtime_error("FileSystemDB") << "StorageProvider::FileSystemDB::SearchIndex Failed creating "
                                           "SearchIndex, path=<"
                                        << index_path.c_str() << ">";
  }

  auto json = std::string{};
  db::read_buffer_from_file(json, {index_path.c_str()});

  TRACE_(3, "StorageProvider::FileSystemDB::index::_open() json=<" << json << ">");

  auto reader = JsonReader{};

  if (reader.read(json, _index)) return true;

  if (shouldAutoRebuildSearchIndex() && _rebuild(index_path)) return true;

  TRACE_(3,
         "StorageProvider::FileSystemDB::index::_open() SearchIndex is "
         "corrupt and needs to be rebuilt.");

  throw runtime_error("FileSystemDB") << "StorageProvider::FileSystemDB::SearchIndex SearchIndex is corrupt "
                                         "and needs to be rebuilt, path=<"
                                      << index_path.c_str() << ">";
  return false;
}

bool SearchIndex::_rebuild(boost::filesystem::path const& index_path) {
  TRACE_(13, "StorageProvider::FileSystemDB::index::_rebuld() begin");
  TRACE_(13, "StorageProvider::FileSystemDB::index::_rebuld() args index_path=<" << index_path.c_str() << ">.");

  try {
    if (!_create(index_path))
      TRACE_(13, "Exception in StorageProvider::FileSystemDB::index::_rebuild() "
                     << ::debug::current_exception_diagnostic_information());

    if (!_open(index_path))
      TRACE_(13, "Exception in StorageProvider::FileSystemDB::index::_rebuild() "
                     << ::debug::current_exception_diagnostic_information());

    auto files = dbfs::list_files_in_directory(index_path.parent_path(), "");

    TRACE_(13, "StorageProvider::FileSystemDB::index::_rebuld() found " << files.size() << " existing documents in "
                                                                        << index_path.parent_path().string());

    for (auto const& file : files) {
      if (file.filename() == "index.json") continue;

      TRACE_(13, "StorageProvider::FileSystemDB::index::_rebuld() Adding " << file.c_str());

      std::ifstream is(file.c_str());

      std::string json((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());

      is.close();

      addDocument({json}, file.stem().string());
    }

    _isDirty = true;
    _isOpen = true;

    return true;
  } catch (...) {
    TRACE_(13,
           "StorageProvider::FileSystemDB::index::_rebuild() Failed to "
           "rebuild SearchIndex; error:"
               << ::debug::current_exception_diagnostic_information());
    return false;
  }
}

bool SearchIndex::_close() {
  if (!_isOpen || !_isDirty) return true;

  TRACE_(4, "StorageProvider::FileSystemDB::index::_close() begin");

  auto writer = JsonWriter{};

  auto json = std::string();

  if (!writer.write(_index, json)) {
    TRACE_(4,
           "StorageProvider::FileSystemDB::index::_close() Failed to close "
           "SearchIndex.");
    return false;
  }

  TRACE_(4, "StorageProvider::FileSystemDB::index::_close() json=<" << json << ">");

  try {
    auto result = db::write_buffer_to_file(json, {_path.c_str()});
    TRACE_(4, "StorageProvider::FileSystemDB::index::_close() Closed SearchIndex.");

    return result;
  } catch (...) {
    TRACE_(4,
           "StorageProvider::FileSystemDB::index::_close() Failed to write "
           "SearchIndex; error:"
               << ::debug::current_exception_diagnostic_information());
    return false;
  }
}

void SearchIndex::_addVersion(object_id_t const& ouid, std::string const& version) {
  confirm(!ouid.empty());
  confirm(!version.empty());

  TRACE_(7, "StorageProvider::FileSystemDB::index::_addVersion() begin");
  TRACE_(7, "StorageProvider::FileSystemDB::index::_addVersion() args ouid=<" << ouid << ">.");
  TRACE_(7, "StorageProvider::FileSystemDB::index::_addVersion() args version=<" << version << ">.");

  auto& versions = boost::get<jsn::object_t>(_index.at("version"));

  auto& version_ouid_list = versions[version];

  if (version_ouid_list.type() == typeid(bool)) version_ouid_list = jsn::array_t{};

  auto& ouids = boost::get<jsn::array_t>(version_ouid_list);
  ouids.push_back(ouid);

  _make_unique_sorted<std::string>(ouids);
}

void SearchIndex::_addId(object_id_t const& ouid) {
  confirm(!ouid.empty());

  TRACE_(7, "StorageProvider::FileSystemDB::index::_addId() begin");
  TRACE_(7, "StorageProvider::FileSystemDB::index::_addId() args ouid=<" << ouid << ">.");

  auto& ouids = boost::get<jsn::array_t>(_index.at("ouid"));
  ouids.push_back(ouid);

  _make_unique_sorted<std::string>(ouids);
}

void SearchIndex::_addConfiguration(object_id_t const& ouid, std::string const& configuration) {
  confirm(!ouid.empty());
  confirm(!configuration.empty());

  TRACE_(7, "StorageProvider::FileSystemDB::index::_addConfiguration() begin");
  TRACE_(7, "StorageProvider::FileSystemDB::index::_addConfiguration() args ouid=<" << ouid << ">.");
  TRACE_(7,
         "StorageProvider::FileSystemDB::index::_addConfiguration() args "
         "configuration=<"
             << configuration << ">.");

  auto& configurations = boost::get<jsn::object_t>(_index.at("configurations.name"));
  auto& configuration_ouid_list = configurations[configuration];

  if (configuration_ouid_list.type() == typeid(bool)) configuration_ouid_list = jsn::array_t{};

  auto& ouids = boost::get<jsn::array_t>(configuration_ouid_list);
  ouids.push_back(ouid);

  _make_unique_sorted<std::string>(ouids);
}

void SearchIndex::_addEntity(object_id_t const& ouid, std::string const& entity) {
  confirm(!ouid.empty());
  confirm(!entity.empty());

  TRACE_(7, "StorageProvider::FileSystemDB::index::_addEntity() begin");
  TRACE_(7,
         "StorageProvider::FileSystemDB::index::_addEntity() "
         "args ouid=<"
             << ouid << ">.");
  TRACE_(7,
         "StorageProvider::FileSystemDB::index::_addEntity() "
         "args entity=<"
             << entity << ">.");

  auto& entities = boost::get<jsn::object_t>(_index.at("entities.name"));
  auto& entity_ouid_list = entities[entity];

  if (entity_ouid_list.type() == typeid(bool)) entity_ouid_list = jsn::array_t{};

  auto& ouids = boost::get<jsn::array_t>(entity_ouid_list);
  ouids.push_back(ouid);

  _make_unique_sorted<std::string>(ouids);
}

void SearchIndex::_addVersionAlias(object_id_t const& ouid, std::string const& veralias) {
  confirm(!ouid.empty());
  confirm(!veralias.empty());

  TRACE_(7, "StorageProvider::FileSystemDB::index::_addVersionAlias() begin");
  TRACE_(7,
         "StorageProvider::FileSystemDB::index::_addVersionAlias() "
         "args ouid=<"
             << ouid << ">.");
  TRACE_(7,
         "StorageProvider::FileSystemDB::index::_addVersionAlias() "
         "args veralias=<"
             << veralias << ">.");

  auto& aliases = boost::get<jsn::object_t>(_index.at("aliases.active.name"));
  auto& alias_ouid_list = aliases[veralias];

  if (alias_ouid_list.type() == typeid(bool)) alias_ouid_list = jsn::array_t{};

  auto& ouids = boost::get<jsn::array_t>(alias_ouid_list);
  ouids.push_back(ouid);

  _make_unique_sorted<std::string>(ouids);
}

void SearchIndex::_addRun(object_id_t const& ouid, std::string const& run) {
  confirm(!ouid.empty());
  confirm(!run.empty());

  TRACE_(7, "StorageProvider::FileSystemDB::index::_addRun() begin");
  TRACE_(7,
         "StorageProvider::FileSystemDB::index::_addRun() "
         "args ouid=<"
             << ouid << ">.");
  TRACE_(7,
         "StorageProvider::FileSystemDB::index::_addRun() "
         "args run=<"
             << run << ">.");

  auto& runs = boost::get<jsn::object_t>(_index.at("runs.name"));
  auto& runs_ouid_list = runs[run];

  if (runs_ouid_list.type() == typeid(bool)) runs_ouid_list = jsn::array_t{};

  auto& ouids = boost::get<jsn::array_t>(runs_ouid_list);
  ouids.push_back(ouid);

  _make_unique_sorted<std::string>(ouids);
}

std::vector<object_id_t> SearchIndex::_matchVersion(std::string const& version) const {
  auto ouids = std::vector<object_id_t>{};

  confirm(!version.empty());

  TRACE_(14, "StorageProvider::FileSystemDB::index::_matchVersion() begin");
  TRACE_(14, "StorageProvider::FileSystemDB::index::_matchVersion() args version=<" << version << ">.");

  auto const& versions = boost::get<jsn::object_t>(_index.at("version"));

  try {
    auto const& version_ouid_list = boost::get<jsn::array_t>(versions.at(version));

    ouids.reserve(version_ouid_list.size());

    for (auto const& version_ouid : version_ouid_list) {
      ouids.push_back(unwrap(version_ouid).value_as<const std::string>());
    }
    TRACE_(15, "StorageProvider::FileSystemDB::index::_matchVersion() Found "
                   << ouids.size() << " documents where version=" << version);

  } catch (std::out_of_range const&) {
    TRACE_(14,
           "StorageProvider::FileSystemDB::index::_matchVersion() Version "
           "not found, version=<"
               << version << ">.");
  }

  return ouids;
}

std::vector<object_id_t> SearchIndex::getObjectIds() const {
  auto ouids = std::vector<object_id_t>{};

  TRACE_(14, "StorageProvider::FileSystemDB::index::getObjectIds() begin");

  try {
    auto const& ouid_list = boost::get<jsn::array_t>(_index.at("ouid"));

    ouids.reserve(ouid_list.size());

    for (auto const& ouid : ouid_list) {
      ouids.push_back(unwrap(ouid).value_as<const std::string>());
    }
    TRACE_(15, "StorageProvider::FileSystemDB::index::getObjectIds() Found " << ouids.size() << " documents");

  } catch (std::out_of_range const&) {
    TRACE_(14, "StorageProvider::FileSystemDB::index::getObjectIds() SearchIndex is corrupt");
  }

  return ouids;
}

std::vector<object_id_t> SearchIndex::_matchVersionAlias(std::string const& veralias) const {
  auto ouids = std::vector<object_id_t>{};

  confirm(!veralias.empty());

  TRACE_(14, "StorageProvider::FileSystemDB::index::_matchVersionAlias() begin");
  TRACE_(14, "StorageProvider::FileSystemDB::index::_matchVersionAlias() args version=<" << veralias << ">.");

  auto const& versions = boost::get<jsn::object_t>(_index.at("aliases.active.name"));

  try {
    auto const& veralias_ouid_list = boost::get<jsn::array_t>(versions.at(veralias));

    ouids.reserve(veralias_ouid_list.size());

    for (auto const& veralias_ouid : veralias_ouid_list) {
      ouids.push_back(unwrap(veralias_ouid).value_as<const std::string>());
    }
    TRACE_(15, "StorageProvider::FileSystemDB::index::_matchVersionAlias() Found "
                   << ouids.size() << " documents where veralias=" << veralias);

  } catch (std::out_of_range const&) {
    TRACE_(14,
           "StorageProvider::FileSystemDB::index::_matchVersionAlias() Version "
           "not found, veralias=<"
               << veralias << ">.");
  }

  return ouids;
}

std::vector<object_id_t> SearchIndex::_matchRun(std::string const& run) const {
  auto ouids = std::vector<object_id_t>{};

  confirm(!run.empty());

  TRACE_(14, "StorageProvider::FileSystemDB::index::_matchRun() begin");
  TRACE_(14, "StorageProvider::FileSystemDB::index::_matchRun() args version=<" << run << ">.");

  auto const& versions = boost::get<jsn::object_t>(_index.at("runs.name"));

  try {
    auto const& run_ouid_list = boost::get<jsn::array_t>(versions.at(run));

    ouids.reserve(run_ouid_list.size());

    for (auto const& run_ouid : run_ouid_list) {
      ouids.push_back(unwrap(run_ouid).value_as<const std::string>());
    }
    TRACE_(15, "StorageProvider::FileSystemDB::index::_matchRun() Found " << ouids.size()
                                                                          << " documents where run=" << run);

  } catch (std::out_of_range const&) {
    TRACE_(14,
           "StorageProvider::FileSystemDB::index::_matchRun() Version "
           "not found, run=<"
               << run << ">.");
  }

  return ouids;
}

std::vector<object_id_t> SearchIndex::_matchConfiguration(std::string const& configuration) const {
  auto ouids = std::vector<object_id_t>{};

  confirm(!configuration.empty());

  TRACE_(15, "StorageProvider::FileSystemDB::index::_matchConfiguration() begin");
  TRACE_(15,
         "StorageProvider::FileSystemDB::index::_matchConfiguration() args "
         "configuration=<"
             << configuration << ">.");

  auto const& versions = boost::get<jsn::object_t>(_index.at("configurations.name"));

  try {
    auto const& configuration_ouid_list = boost::get<jsn::array_t>(versions.at(configuration));

    ouids.reserve(configuration_ouid_list.size());

    for (auto const& configuration_ouid : configuration_ouid_list) {
      ouids.push_back(unwrap(configuration_ouid).value_as<const std::string>());
    }
    TRACE_(15, "StorageProvider::FileSystemDB::index::_matchConfiguration() Found "
                   << ouids.size() << " documents where configuration=" << configuration);

  } catch (std::out_of_range const&) {
    TRACE_(15,
           "StorageProvider::FileSystemDB::index::_matchConfiguration() "
           "Configuration not found, configuration=<"
               << configuration << ">.");
  }

  return ouids;
}

std::vector<object_id_t> SearchIndex::_matchEntity(std::string const& entity) const {
  auto ouids = std::vector<object_id_t>{};

  confirm(!entity.empty());

  TRACE_(15, "StorageProvider::FileSystemDB::index::_matchEntity() begin");
  TRACE_(15,
         "StorageProvider::FileSystemDB::index::_matchEntity() "
         "args entities=<"
             << entity << ">.");

  auto const& entities = boost::get<jsn::object_t>(_index.at("entities.name"));

  try {
    auto const& entity_ouid_list = boost::get<jsn::array_t>(entities.at(entity));

    ouids.reserve(entity_ouid_list.size());

    for (auto const& entity_ouid : entity_ouid_list) {
      ouids.push_back(unwrap(entity_ouid).value_as<const std::string>());
    }
    TRACE_(15,
           "StorageProvider::FileSystemDB::index::_matchEntity("
           ") Found "
               << ouids.size() << " documents where entity=" << entity);
  } catch (std::out_of_range const&) {
    TRACE_(15,
           "StorageProvider::FileSystemDB::index::_matchEntity("
           ") Configuration entity not found, entity=<"
               << entity << ">.");
  }

  return ouids;
}

void SearchIndex::_removeVersion(object_id_t const& old_ouid, std::string const& version) {
  confirm(!old_ouid.empty());
  confirm(!version.empty());

  TRACE_(7, "StorageProvider::FileSystemDB::index::_removeVersion() begin");
  TRACE_(7, "StorageProvider::FileSystemDB::index::_removeVersion() args ouid=<" << old_ouid << ">.");
  TRACE_(7, "StorageProvider::FileSystemDB::index::_removeVersion() args version=<" << version << ">.");

  auto& versions = boost::get<jsn::object_t>(_index.at("version"));

  try {
    auto& version_ouid_list = boost::get<jsn::array_t>(versions.at(version));
    auto new_version_ouid_list = jsn::array_t{};

    for (auto& version_ouid : version_ouid_list) {
      auto this_ouid = unwrap(version_ouid).value_as<std::string>();

      if (this_ouid != old_ouid) {
        new_version_ouid_list.push_back(this_ouid);
      } else {
        TRACE_(7,
               "StorageProvider::FileSystemDB::index::_removeVersion() "
               "removed ouid=<"
                   << old_ouid << ">.");
      }
    }

    version_ouid_list.swap(new_version_ouid_list);
  } catch (std::out_of_range const&) {
    TRACE_(7,
           "StorageProvider::FileSystemDB::index::_removeVersion() Version "
           "not found, version=<"
               << version << ">.");
  }
}

void SearchIndex::_removeId(object_id_t const& old_ouid) {
  confirm(!old_ouid.empty());

  TRACE_(7, "StorageProvider::FileSystemDB::index::_removeId() begin");
  TRACE_(7, "StorageProvider::FileSystemDB::index::_removeId() args ouid=<" << old_ouid << ">.");

  auto& ouid_list = boost::get<jsn::array_t>(_index.at("ouid"));

  try {
    auto new_ouid_list = jsn::array_t{};

    for (auto& ouid : ouid_list) {
      auto this_ouid = unwrap(ouid).value_as<std::string>();

      if (this_ouid != old_ouid) {
        new_ouid_list.push_back(this_ouid);
      } else {
        TRACE_(7,
               "StorageProvider::FileSystemDB::index::_removeId() "
               "removed ouid=<"
                   << old_ouid << ">.");
      }
    }
    ouid_list.swap(new_ouid_list);
  } catch (std::out_of_range const&) {
    TRACE_(7,
           "StorageProvider::FileSystemDB::index::_removeId() ouid "
           "not found, ouid=<"
               << old_ouid << ">.");
  }
}

void SearchIndex::_removeVersionAlias(object_id_t const& old_ouid, std::string const& veralias) {
  confirm(!old_ouid.empty());
  confirm(!veralias.empty());

  TRACE_(7, "StorageProvider::FileSystemDB::index::_removeVersionAlias() begin");
  TRACE_(7, "StorageProvider::FileSystemDB::index::_removeVersionAlias() args ouid=<" << old_ouid << ">.");
  TRACE_(7, "StorageProvider::FileSystemDB::index::_removeVersionAlias() args veralias=<" << veralias << ">.");

  auto& veraliass = boost::get<jsn::object_t>(_index.at("aliases.active.name"));

  try {
    auto& veralias_ouid_list = boost::get<jsn::array_t>(veraliass.at(veralias));
    auto new_veralias_ouid_list = jsn::array_t{};

    for (auto& veralias_ouid : veralias_ouid_list) {
      auto this_ouid = unwrap(veralias_ouid).value_as<std::string>();

      if (this_ouid != old_ouid) {
        new_veralias_ouid_list.push_back(this_ouid);
      } else {
        TRACE_(7,
               "StorageProvider::FileSystemDB::index::_removeVersionAlias() "
               "removed ouid=<"
                   << old_ouid << ">.");
      }
    }
    veralias_ouid_list.swap(new_veralias_ouid_list);
  } catch (std::out_of_range const&) {
    TRACE_(7,
           "StorageProvider::FileSystemDB::index::_removeVersionAlias() VersionAlias "
           "not found, veralias=<"
               << veralias << ">.");
  }
}

void SearchIndex::_removeRun(object_id_t const& old_ouid, std::string const& run) {
  confirm(!old_ouid.empty());
  confirm(!run.empty());

  TRACE_(7, "StorageProvider::FileSystemDB::index::_removeRun() begin");
  TRACE_(7, "StorageProvider::FileSystemDB::index::_removeRun() args ouid=<" << old_ouid << ">.");
  TRACE_(7, "StorageProvider::FileSystemDB::index::_removeRun() args run=<" << run << ">.");

  auto& runs = boost::get<jsn::object_t>(_index.at("runs.name"));

  try {
    auto& run_ouid_list = boost::get<jsn::array_t>(runs.at(run));
    auto new_run_ouid_list = jsn::array_t{};

    for (auto& run_ouid : run_ouid_list) {
      auto this_ouid = unwrap(run_ouid).value_as<std::string>();

      if (this_ouid != old_ouid) {
        new_run_ouid_list.push_back(this_ouid);
      } else {
        TRACE_(7,
               "StorageProvider::FileSystemDB::index::_removeRun() "
               "removed ouid=<"
                   << old_ouid << ">.");
      }
    }
    run_ouid_list.swap(new_run_ouid_list);
  } catch (std::out_of_range const&) {
    TRACE_(7,
           "StorageProvider::FileSystemDB::index::_removeRun() Run "
           "not found, run=<"
               << run << ">.");
  }
}

std::vector<object_id_t> SearchIndex::_matchObjectId(std::string const& objectid) const {
  auto ouids = std::vector<object_id_t>{};

  confirm(!objectid.empty());

  TRACE_(16, "StorageProvider::FileSystemDB::index::_matchObjectId() begin");
  TRACE_(16, "StorageProvider::FileSystemDB::index::_matchObjectId() args objectid=<" << objectid << ">.");

  ouids.push_back(objectid);

  return ouids;
}

std::vector<object_id_t> SearchIndex::_matchObjectIds(std::string const& objectids) const {
  auto ouids = std::vector<object_id_t>{};

  confirm(!objectids.empty());

  TRACE_(16, "StorageProvider::FileSystemDB::index::_matchObjectIds() begin");
  TRACE_(16, "StorageProvider::FileSystemDB::index::_matchObjectIds() args objectid=<" << objectids << ">.");

  auto reader = JsonReader{};

  object_t objectids_ast;

  if (!reader.read(objectids, objectids_ast)) {
    TRACE(16, "StorageProvider::FileSystemDB::index::_matchObjectIds() Failed to create an AST from objectids.");
    return ouids;
  }

  try {
    auto ouid_list = boost::get<jsn::array_t>(objectids_ast.at("$in"));

    TRACE(16, "StorageProvider::FileSystemDB::index::_matchObjectIds() found %lu ouids.", ouid_list.size());

    for (auto& ouid_entry : ouid_list) {
      auto ouid = boost::get<std::string>(boost::get<jsn::object_t>(ouid_entry).at("$oid"));
      TRACE(16, "StorageProvider::FileSystemDB::index::_matchObjectIds() found ouid=<%s>.", ouid.c_str());
      ouids.push_back(ouid);
    }
  } catch (...) {
    TRACE(16, "StorageProvider::FileSystemDB::index::_matchObjectIds() has no $in");
  }

  try {
    auto ouid = boost::get<std::string>(boost::get<jsn::object_t>(objectids_ast).at("$oid"));
    TRACE(16, "StorageProvider::FileSystemDB::index::_matchObjectIds() found ouid=<%s>.", ouid.c_str());
    ouids.push_back(ouid);
  } catch (...) {
    TRACE(16, "StorageProvider::FileSystemDB::index::_matchObjectIds() has no $oid");
  }

  TRACE(16, "StorageProvider::FileSystemDB::index::_matchObjectIds() Found %lu documents", ouids.size());

  return ouids;
}
void SearchIndex::_removeConfiguration(object_id_t const& old_ouid, std::string const& configuration) {
  confirm(!old_ouid.empty());
  confirm(!configuration.empty());

  TRACE_(8, "StorageProvider::FileSystemDB::index::_removeConfiguration() begin");
  TRACE_(8, "StorageProvider::FileSystemDB::index::_removeConfiguration() args ouid=<" << old_ouid << ">.");
  TRACE_(8,
         "StorageProvider::FileSystemDB::index::_removeConfiguration() args "
         "configuration=<"
             << configuration << ">.");

  auto& configurations = boost::get<jsn::object_t>(_index.at("configurations.name"));

  try {
    auto& configuration_ouid_list = boost::get<jsn::array_t>(configurations.at(configuration));
    auto new_configuration_ouid_list = jsn::array_t{};

    for (auto& configuration_ouid : configuration_ouid_list) {
      auto this_ouid = unwrap(configuration_ouid).value_as<std::string>();

      if (this_ouid != old_ouid) {
        new_configuration_ouid_list.push_back(this_ouid);
      } else {
        TRACE_(8,
               "StorageProvider::FileSystemDB::index::_removeConfiguration()"
               " removed ouid=<"
                   << old_ouid << ">.");
      }
    }
    configuration_ouid_list.swap(new_configuration_ouid_list);
  } catch (std::out_of_range const&) {
    TRACE_(8,
           "StorageProvider::FileSystemDB::index::_removeConfiguration() "
           "Configuration not found, configuration=<"
               << configuration << ">.");
  }
}

void SearchIndex::_removeEntity(object_id_t const& old_ouid, std::string const& entity) {
  confirm(!old_ouid.empty());
  confirm(!entity.empty());

  TRACE_(8,
         "StorageProvider::FileSystemDB::index::_removeEntity() "
         "begin");
  TRACE_(8,
         "StorageProvider::FileSystemDB::index::_removeEntity() "
         "args ouid=<"
             << old_ouid << ">.");
  TRACE_(8,
         "StorageProvider::FileSystemDB::index::_removeEntity() "
         "args entity=<"
             << entity << ">.");

  auto& entities = boost::get<jsn::object_t>(_index.at("entities.name"));

  try {
    auto& entity_ouid_list = boost::get<jsn::array_t>(entities.at(entity));
    auto new_entity_ouid_list = jsn::array_t{};

    for (auto& entity_ouid : entity_ouid_list) {
      auto this_ouid = unwrap(entity_ouid).value_as<std::string>();

      if (this_ouid != old_ouid) {
        new_entity_ouid_list.push_back(this_ouid);
      } else {
        TRACE_(8,
               "StorageProvider::FileSystemDB::index::_"
               "_removeEntity() removed ouid=<"
                   << old_ouid << ">.");
      }
    }
    entity_ouid_list.swap(new_entity_ouid_list);
  } catch (std::out_of_range const&) {
    TRACE_(8,
           "StorageProvider::FileSystemDB::index::_removeEntity("
           ") Configuration not found, entity=<"
               << entity << ">.");
  }
}

void SearchIndex::_build_ouid_map(std::map<std::string, std::string>& map, std::string const& what) const {
  confirm(!what.empty());
  confirm(map.empty());

  auto ouid_map = std::map<std::string, std::string>{};

  auto& lookup_table = boost::get<jsn::object_t>(_index.at(what));

  for (auto& ouid_array_element : lookup_table) {
    auto& ouid_list = boost::get<jsn::array_t>(ouid_array_element.value);

    for (auto& ouid : ouid_list) {
      ouid_map[unwrap(ouid).value_as<const std::string>()] = ouid_array_element.key;
    }
  }

  TRACE_(5, "StorageProvider::FileSystemDB::index::_build_ouid2entity_map()"
                << " ouid2entity_map has " << ouid_map.size() << " entries.");

  map.swap(ouid_map);
}

std::vector<std::string> SearchIndex::_filtered_attribute_list(std::string const& attribute,
                                                               std::string const& attribute_begins_with) const {
  confirm(!attribute.empty());

  auto returnCollection = std::vector<std::string>{};

  auto acceptValue = [&attribute_begins_with](auto const& value) {
    if (attribute_begins_with.empty()) return true;

    if (value.size() < attribute_begins_with.size() /*|| value == "notprovided"*/) return false;

    auto first(std::begin(attribute_begins_with)), last(std::end(attribute_begins_with));

    if (attribute_begins_with.back() != '*') {
      if (value.size() == attribute_begins_with.size() && std::equal(first, last, value.begin())) return true;
    } else {
      if (std::equal(first, std::prev(last), value.begin())) return true;
    }

    // if (std::equal(attribute_begins_with.begin(),
    // attribute_begins_with.end(), value.begin())) return true;

    return false;
  };

  auto& lookup_table = boost::get<jsn::object_t>(_index.at(attribute));

  for (auto& ouid_array_element : lookup_table) {
    if (!acceptValue(ouid_array_element.key)) continue;

    returnCollection.push_back(ouid_array_element.key);
  }

  TRACE_(5, "StorageProvider::FileSystemDB::index::_filtered_attribute_list()"
                << " filtered_attribute_list has " << returnCollection.size() << " entries.");

  return returnCollection;
}

std::vector<std::pair<std::string, std::string>> SearchIndex::_indexed_filtered_innerjoin_over_ouid(
    std::string const& left, std::string const& right, std::string const& right_begins_with) const {
  auto returnCollection = std::vector<std::pair<std::string, std::string>>{};

  auto acceptValue = [&right_begins_with](auto const& value) {
    if (value.size() < right_begins_with.size() /*|| value == "notprovided"*/) return false;

    auto first(std::begin(right_begins_with)), last(std::end(right_begins_with));

    if (right_begins_with.back() != '*') {
      if (value.size() == right_begins_with.size() && std::equal(first, last, value.begin())) return true;
    } else {
      if (std::equal(first, std::prev(last), value.begin())) return true;
    }

    return false;
  };

  auto ouid_map = std::map<std::string, std::string>{};

  _build_ouid_map(ouid_map, left);

  auto& right_table = boost::get<jsn::object_t>(_index.at(right));

  for (auto& right_element : right_table) {
    if (!acceptValue(right_element.key)) continue;

    TRACE_(5,
           "StorageProvider::FileSystemDB::index::_indexed_filtered_"
           "innerjoin_over_ouid()"
               << " accepted value  " << right_element.key << " begins with " << right_begins_with);

    auto& right_ouid_list = boost::get<jsn::array_t>(right_element.value);

    for (auto& right_ouid : right_ouid_list) {
      returnCollection.emplace_back(right_element.key, ouid_map.at(unwrap(right_ouid).value_as<const std::string>()));
    }
  }

  if (returnCollection.size() < 2) return returnCollection;

  std::sort(returnCollection.begin(), returnCollection.end());

  auto unique_ouids = std::vector<std::pair<std::string, std::string>>{};

  std::unique_copy(returnCollection.begin(), returnCollection.end(), back_inserter(unique_ouids));

  if (!unique_ouids.empty()) returnCollection.swap(unique_ouids);

  TRACE_(5,
         "StorageProvider::FileSystemDB::index::_indexed_filtered_innerjoin_"
         "over_ouid()"
             << " returnCollection has " << returnCollection.size() << " entries.");

  return returnCollection;
}

void dbfsi::debug::enable() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TRACE_(0, "artdaq::database::filesystem::index trace_enable");
}

using artdaq::database::sharedtypes::unwrap;
using artdaq::database::sharedtypes::unwrapper;

template <>
template <typename T>
T& unwrapper<jsn::value_t>::value_as() {
  return boost::get<T>(any);
}

template <>
template <typename T>
T const& unwrapper<const jsn::value_t>::value_as() {
  using V = typename std::remove_const<T>::type;

  return boost::get<V>(any);
}

bool dbfsi::shouldAutoRebuildSearchIndex(bool rebuild) {
  static const bool _shouldAutoRebuildSearchIndex = rebuild;
  return _shouldAutoRebuildSearchIndex;
}