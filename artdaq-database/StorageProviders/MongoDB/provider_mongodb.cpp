#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/StorageProviders/MongoDB/mongo_json.h"
#include "artdaq-database/StorageProviders/MongoDB/provider_mongodb.h"
#include "artdaq-database/StorageProviders/common.h"

#include "artdaq-database/SharedCommon/sharedcommon_common.h"

#include <bsoncxx/builder/basic/helpers.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/types/value.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/options/find.hpp>
#include <mongocxx/pipeline.hpp>

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "PRVDR:MongoDB_C"

using namespace artdaq::database;
namespace db = artdaq::database;

namespace literal = db::mongo::literal;
namespace bbs = bsoncxx::builder::stream;

using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::concatenate_doc;

using artdaq::database::mongo::DBConfig;
using artdaq::database::mongo::MongoDB;
using artdaq::database::basictypes::JsonData;

DBConfig::DBConfig()
    : uri{std::string{literal::MONGOURI} + literal::hostname + ":" + literal::port + "/" + literal::db_name} {
  auto tmpURI =
      getenv("ARTDAQ_DATABASE_URI") ? expand_environment_variables("${ARTDAQ_DATABASE_URI}") : std::string("");

  if (tmpURI.back() == '/') tmpURI.pop_back();  // remove trailing slash

  auto prefixURI = std::string{literal::MONGOURI};
  if (tmpURI.length() > prefixURI.length() && std::equal(prefixURI.begin(), prefixURI.end(), tmpURI.begin()))
    uri = tmpURI;
}

DBConfig::DBConfig(std::string uri_) : uri{uri_} { confirm(!uri_.empty()); }

MongoDB::MongoDB(DBConfig const& config, PassKeyIdiom const&)
    : _config{config},
      _instance{},
      _client{mongocxx::uri{_config.connectionURI()}},
      _connection{_client[_client.uri().database()]} {}

mongocxx::cursor MongoDB::list_databases() {
  connection();
  return _client.list_databases();
}

mongocxx::database& MongoDB::connection() {
  // if (_connection.has_collection(system_metadata)) return _connection;

  auto collection = _connection.collection(system_metadata);
  auto filter = bsoncxx::builder::core(false);

  if (collection.count(filter.view_document()) > 0) return _connection;

  auto bson_document = bsoncxx::from_json(
      "{" + make_database_metadata("artdaq", expand_environment_variables(_config.connectionURI())) + "}");

  auto result = collection.insert_one(bson_document.view());

  auto object_id = object_id_t(bsoncxx::to_json(result->inserted_id()));

  TRACE_(5, "StorageProvider::MongoDB::connection created metadata record id=" << object_id);

  return _connection;
}

namespace artdaq {
namespace database {
template <>
template <>
std::list<JsonData> StorageProvider<JsonData, MongoDB>::load(JsonData const& search) {
  TRACE_(3, "StorageProvider::MongoDB::load() begin");
  TRACE_(3, "StorageProvider::MongoDB::load() args search=<" << search.json_buffer << ">");

  auto returnCollection = std::list<JsonData>();

  auto bson_document = bsoncxx::from_json(search.json_buffer);

  auto extract_value = [&bson_document](auto const& name) {
    auto view = bson_document.view();
    auto element = view.find(name);

    if (element == view.end())
      throw runtime_error("MongoDB") << "Search JsonData is missing the \"" << name << "\" element.";

    return element->get_value();
  };

  auto collection_name = dequote(bsoncxx::to_json(extract_value("collection")));

  auto collection = _provider->connection().collection(collection_name);

  auto filter = bsoncxx::builder::core(false);

  try {
    auto tmp = extract_value("filter");

    TRACE_(3, "StorageProvider::MongoDB::load() user provided filter=<" << bsoncxx::to_json(tmp) << ">");

    using value_type_t = bsoncxx::type;
    using namespace bsoncxx::builder::stream;

    switch (static_cast_as_uint8_t(tmp.type())) {
      default: {
        TRACE_(3, "StorageProvider::MongoDB::load() case  default=<" << bsoncxx::to_json(tmp) << ">");

        filter.append(tmp);
        break;
      }
      case static_cast_as_uint8_t(value_type_t::k_document): {
        auto doc = bbs::document{};
        doc << concatenate_doc{tmp.get_document().value};
        filter.concatenate(doc);
        break;
      }
      case static_cast_as_uint8_t(value_type_t::k_oid): {
        auto doc = document{};
        doc << "_id" << open_document << "$in" << open_array << tmp << close_array << close_document;
        filter.concatenate(doc);
        break;
      }
    }
    TRACE_(2, "collection_name=\"" << collection_name << "\", search filter=<"
                                   << bsoncxx::to_json(filter.view_document()) << ">");

  } catch (cet::exception const&) {
    TRACE_(2, "search filter is missing");
  } catch (std::exception const& e) {
    TRACE_(2, "Caught exception:" << e.what());
    throw runtime_error("MongoDB") << "Caught exception:" << e.what();
  }

  auto size = collection.count(filter.view_document());
  TRACE_(3, "found_count=" << size);

  auto cursor = collection.find(filter.view_document());

  for (auto& doc : cursor) {
    TRACE_(3, "found_document=<" << artdaq::database::mongo::to_json(doc) << ">");

    returnCollection.emplace_back(artdaq::database::mongo::to_json(doc));
  }
  return returnCollection;
}

template <>
object_id_t StorageProvider<JsonData, MongoDB>::store(JsonData const& data) {
  TRACE_(4, "StorageProvider::MongoDB::store() begin");

  auto bson_document = bsoncxx::from_json(data.json_buffer);

  auto extract_value = [&bson_document](auto const& name) {
    auto view = bson_document.view();
    auto element = view.find(name);

    if (element == view.end())
      throw runtime_error("MongoDB") << "Search JsonData is missing the \"" << name << "\" element.";

    return element->get_value();
  };

  auto collection_name = dequote(bsoncxx::to_json(extract_value("collection")));
  auto collection = _provider->connection().collection(collection_name);

  auto value = extract_value("document");

  auto filter = bsoncxx::builder::core(false);

  auto isNew = bool{true};
  auto replaceOne = bool{false};
  try {
    auto tmp = extract_value("filter");

    TRACE_(3, "StorageProvider::MongoDB::load() user provided filter=<" << bsoncxx::to_json(tmp) << ">");

    
    using value_type_t = bsoncxx::type;
    using namespace bsoncxx::builder::stream;

    switch (static_cast_as_uint8_t(tmp.type())) {
      default: {
        TRACE_(3, "StorageProvider::MongoDB::load() case  default=<" << bsoncxx::to_json(tmp) << ">");

        filter.append(tmp);
        break;
      }
      case static_cast_as_uint8_t(value_type_t::k_document): {
        auto doc = bbs::document{};
        doc << concatenate_doc{tmp.get_document().value};
        filter.concatenate(doc);
        break;
      }
      case static_cast_as_uint8_t(value_type_t::k_oid): {
        auto doc = bbs::document{};
        doc << "_id" << open_document << "$in" << open_array << tmp << close_array << close_document;
        filter.concatenate(doc);
        replaceOne = true;
        break;
      }
    }
    TRACE_(2, "collection_name=\"" << collection_name << "\", search filter=<"
                                   << bsoncxx::to_json(filter.view_document()) << ">");

    isNew = false;
  } catch (cet::exception const&) {
    TRACE_(4, "Search filter is missing, proceeding with insert.");
  }

  if (isNew) {
    auto result = collection.insert_one(value.get_document().value);

    auto object_id = object_id_t(bsoncxx::to_json(result->inserted_id()));

    TRACE_(5, "inserted_id=" << object_id);

    return object_id;
  } else if (replaceOne) {
    auto result[[gnu::unused]] = collection.replace_one(filter.view_document(), value.get_document().value);

    TRACE_(8, "modified_count=" << result->modified_count());
    return "";
  } else {
    auto operation = std::string("$set");
    auto path = extract_value("path");

    auto update = bbs::document{};
    update << "$set" << open_document << "path" << value << close_document;

    TRACE_(2, "update data=<" << bsoncxx::to_json(update) << ">");
    TRACE_(2, "update filter=<" << bsoncxx::to_json(filter.view_document()) << ">");

    auto result[[gnu::unused]] = collection.update_many(filter.view_document(), update.view());

    TRACE_(8, "modified_count=" << result->modified_count());

    return "";
  }
}
template <>
template <>
std::list<JsonData> StorageProvider<JsonData, MongoDB>::findGlobalConfigs(JsonData const& search) {
  confirm(!search.json_buffer.empty());
  auto returnCollection = std::list<JsonData>();

  TRACE_(4, "StorageProvider::MongoDB::findGlobalConfigs() begin");
  TRACE_(4, "StorageProvider::MongoDB::findGlobalConfigs() args data=<" << search.json_buffer << ">");

  auto filter = bsoncxx::builder::core(false);

  auto search_filter_doc = bbs::document{};

  filter.concatenate(search_filter_doc);

  auto collectionDescriptors = _provider->connection().list_collections(filter.view_document());

  for (auto const& collectionDescriptor : collectionDescriptors) {
    TRACE_(4, "StorageProvider::MongoDB::findGlobalConfigs() found collection=<"
                  << bsoncxx::to_json(collectionDescriptor) << ">");

    auto element_name = collectionDescriptor.find("name");

    if (element_name == collectionDescriptor.end())
      throw runtime_error("MongoDB") << "MongoDB returned invalid database collection descriptor.";

    auto string_value = element_name->get_value();

    auto collection_name = dequote(bsoncxx::to_json(string_value));

    if (collection_name == "system.indexes" || collection_name == system_metadata) continue;

    TRACE_(4,
           "StorageProvider::MongoDB::findGlobalConfigs() querying "
           "collection_name=<"
               << collection_name << ">");

    auto collection = _provider->connection().collection(collection_name);

    auto configuration_filter = bsoncxx::builder::core(false);
    auto bson_document = bsoncxx::from_json(search.json_buffer);
    configuration_filter.concatenate(bson_document.view());

    auto cursor = collection.distinct("configurations.name", configuration_filter.view_document());

    for (auto const& view : cursor) {
      TRACE_(4, "StorageProvider::MongoDB::findGlobalConfigs() looping over cursor =<" << bsoncxx::to_json(view)
                                                                                       << ">");

      auto element_values = view.find("values");

      if (element_values == collectionDescriptor.end())
        throw runtime_error("MongoDB") << "MongoDB returned invalid database search.";

      auto configuration_name_array = element_values->get_array();

      if (configuration_name_array.value.empty()) break;

      for (auto const& configuration_name : configuration_name_array.value) {
        std::ostringstream oss;
        oss << "{";
        // oss << "\"collection\" : \"" << collection_name << "\",";
        oss << "\"dbprovider\" : \"mongo\",";
        oss << "\"dataformat\" : \"gui\",";
        oss << "\"operation\" : \"buildfilter\",";
        oss << "\"filter\" : {";
        auto name_json = bsoncxx::to_json(configuration_name.get_value());
        oss << "\"configurations.name\" : " << name_json;
        oss << "}";
        oss << "}";
        TRACE_(4, "StorageProvider::MongoDB::findGlobalConfigs() found document=<" << oss.str() << ">");

        returnCollection.emplace_back(oss.str());
      }
    }
  }

  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, MongoDB>::buildConfigSearchFilter(JsonData const& search) {
  confirm(!search.json_buffer.empty());
  auto returnCollection = std::list<JsonData>();

  TRACE_(5, "StorageProvider::MongoDB::buildConfigSearchFilter() begin");
  TRACE_(5, "StorageProvider::MongoDB::buildConfigSearchFilter() args data=<" << search.json_buffer << ">");

  auto filter = bsoncxx::builder::core(false);

  auto search_filter_doc = bbs::document{};

  filter.concatenate(search_filter_doc);

  auto collectionDescriptors = _provider->connection().list_collections(filter.view_document());

  for (auto const& collectionDescriptor : collectionDescriptors) {
    TRACE_(5, "StorageProvider::MongoDB::buildConfigSearchFilter() found collection=<"
                  << bsoncxx::to_json(collectionDescriptor) << ">");

    // auto view = collectionDescriptor.view();
    auto element_name = collectionDescriptor.find("name");

    if (element_name == collectionDescriptor.end())
      throw runtime_error("MongoDB") << "MongoDB returned invalid database collection descriptor.";

    auto string_value = element_name->get_value();

    auto collection_name = dequote(bsoncxx::to_json(string_value));

    if (collection_name == "system.indexes" || collection_name == system_metadata) continue;

    TRACE_(5,
           "StorageProvider::MongoDB::buildConfigSearchFilter() querying "
           "collection_name=<"
               << collection_name << ">");

    auto collection = _provider->connection().collection(collection_name);
    auto bson_document = bsoncxx::from_json(search.json_buffer);

    mongocxx::pipeline stages;
    bbs::document project_stage;
    auto match_stage = bsoncxx::builder::core(false);

    match_stage.concatenate(bson_document.view());

    project_stage << "_id" << 0 << "configurations"
                  << "$configurations.name"
                  << "configurable_entity"
                  << "$configurable_entity.name";

    bbs::document sort_stage;
    sort_stage << "configurations.name" << 1 << "configurable_entity.name" << 1;

    stages.match(match_stage.view_document()).project(project_stage.view());  //.sort(sort_stage.view());

    TRACE_(5, "StorageProvider::MongoDB::buildConfigSearchFilter()  search_filter=<" << bsoncxx::to_json(stages.view())
                                                                                     << ">");

    auto cursor = collection.aggregate(stages);

    for (auto const& view : cursor) {
      TRACE_(5, "StorageProvider::MongoDB::buildConfigSearchFilter()  value=<" << bsoncxx::to_json(view) << ">");

      auto configurable_entity = view.find("configurable_entity");
      auto configurable_entity_name = bsoncxx::to_json(configurable_entity->get_value());

      auto element_values = view.find("configurations");

      if (element_values == collectionDescriptor.end())
        throw runtime_error("MongoDB") << "MongoDB returned invalid database search.";

      auto configuration_name_array = element_values->get_array();
      for (auto const& configuration_name_element[[gnu::unused]] : configuration_name_array.value) {
        //        auto configuration_name =
        //        bsoncxx::to_json(configuration_name_element.get_value());

        //	if(configuration_name=="notprovided")
        //	  continue;

        auto configuration_name = bsoncxx::to_json(bson_document.view()["configurations.name"].get_value());

        std::ostringstream oss;
        oss << "{";
        oss << "\"collection\" : \"" << collection_name << "\",";
        oss << "\"dbprovider\" : \"mongo\",";
        oss << "\"dataformat\" : \"gui\",";
        oss << "\"operation\" : \"load\",";
        oss << "\"filter\" : {";
        oss << "\"configurations.name\" : " << configuration_name;
        oss << ", \"configurable_entity.name\" : " << configurable_entity_name;
        oss << "}";
        oss << "}";

        TRACE_(4, "StorageProvider::MongoDB::buildConfigSearchFilter() found document=<" << oss.str() << ">");

        returnCollection.emplace_back(oss.str());

        break;
      }
    }
  }

  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, MongoDB>::findConfigVersions(JsonData const& search_filter) {
  confirm(!search_filter.json_buffer.empty());
  auto returnCollection = std::list<JsonData>();

  TRACE_(7, "StorageProvider::MongoDB::findConfigVersions() begin");
  TRACE_(7, "StorageProvider::MongoDB::findConfigVersions() args data=<" << search_filter.json_buffer << ">");

  auto bson_document = bsoncxx::from_json(search_filter.json_buffer);

  auto extract_value = [&bson_document](auto const& name) {
    auto view = bson_document.view();
    auto element = view.find(name);

    if (element == view.end())
      throw runtime_error("MongoDB") << "Search JsonData is missing the \"" << name << "\" element.";

    return element->get_value();
  };

  auto collection_name = dequote(bsoncxx::to_json(extract_value("collection")));

  auto collection = _provider->connection().collection(collection_name);

  mongocxx::pipeline stages;
  bbs::document project_stage;
  auto match_stage = bsoncxx::builder::core(false);
  match_stage.concatenate(extract_value("filter").get_document().value);

  project_stage << "_id" << 0 << "version"
                << "$version"
                << "configurable_entity"
                << "$configurable_entity.name";

  stages.match(match_stage.view_document()).project(project_stage.view());

  TRACE_(5, "StorageProvider::MongoDB::findConfigVersions()  search_filter=<" << bsoncxx::to_json(stages.view())
                                                                              << ">");

  auto cursor = collection.aggregate(stages);

  for (auto const& view : cursor) {
    auto configurable_entity = view.find("configurable_entity");
    auto configurable_entity_name = bsoncxx::to_json(configurable_entity->get_value());
    auto version = view.find("version");
    auto version_name = bsoncxx::to_json(version->get_value());

    std::ostringstream oss;
    oss << "{";
    oss << "\"collection\" : \"" << collection_name << "\",";
    oss << "\"dbprovider\" : \"mongo\",";
    oss << "\"dataformat\" : \"gui\",";
    oss << "\"operation\" : \"load\",";
    oss << "\"filter\" : {";
    oss << "\"version\" : " << version_name;
    oss << ", \"configurable_entity.name\" : " << configurable_entity_name;
    oss << "}";
    oss << "}";

    TRACE_(4, "StorageProvider::MongoDB::findConfigVersions() found document=<" << oss.str() << ">");

    returnCollection.emplace_back(oss.str());
  }

  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, MongoDB>::findConfigEntities(JsonData const& search) {
  confirm(!search.json_buffer.empty());
  auto returnCollection = std::list<JsonData>();

  TRACE_(9, "StorageProvider::MongoDB::findConfigEntities() begin");
  TRACE_(9, "StorageProvider::MongoDB::findConfigEntities() args data=<" << search.json_buffer << ">");

  auto bson_document = bsoncxx::from_json(search.json_buffer);

  auto extract_value = [&bson_document](auto const& name) {
    auto view = bson_document.view();
    auto element = view.find(name);

    if (element == view.end())
      throw runtime_error("MongoDB") << "Search JsonData is missing the \"" << name << "\" element.";

    return element->get_value();
  };

  auto filter = bsoncxx::builder::core(false);
  auto search_filter_doc = bbs::document{};
  filter.concatenate(search_filter_doc);

  auto collectionDescriptors = _provider->connection().list_collections(filter.view_document());

  for (auto const& collectionDescriptor : collectionDescriptors) {
    TRACE_(9, "StorageProvider::MongoDB::findConfigEntities() found collection=<"
                  << bsoncxx::to_json(collectionDescriptor) << ">");

    auto element_name = collectionDescriptor.find("name");

    if (element_name == collectionDescriptor.end())
      throw runtime_error("MongoDB") << "MongoDB returned invalid database collection descriptor.";

    auto string_value = element_name->get_value();

    auto collection_name = dequote(bsoncxx::to_json(string_value));

    if (collection_name == "system.indexes" || collection_name == system_metadata) continue;

    TRACE_(9,
           "StorageProvider::MongoDB::findConfigEntities() querying "
           "collection_name=<"
               << collection_name << ">");

    auto collection = _provider->connection().collection(collection_name);
    auto bson_document = bsoncxx::from_json(search.json_buffer);

    mongocxx::pipeline stages;
    bbs::document project_stage;
    auto match_stage = bsoncxx::builder::core(false);

    match_stage.concatenate(extract_value("filter").get_document().value);

    project_stage << "_id" << 0 << "configurable_entity"
                  << "$configurable_entity.name";

    stages.match(match_stage.view_document()).project(project_stage.view());

    TRACE_(9, "StorageProvider::MongoDB::findConfigEntities()  search_filter=<" << bsoncxx::to_json(stages.view())
                                                                                << ">");
    auto cursor = collection.aggregate(stages);

    auto seenValues = std::list<std::string>{};

    auto isNew = [& v = seenValues](auto const& name) {
      confirm(!name.empty());
      if (std::find(v.begin(), v.end(), name) != v.end()) return false;

      v.emplace_back(name);
      return true;
    };

    for (auto const& view : cursor) {
      TRACE_(9, "StorageProvider::MongoDB::findConfigEntities()  value=<" << bsoncxx::to_json(view) << ">");

      auto configurable_entity = view.find("configurable_entity");
      auto configurable_entity_name = bsoncxx::to_json(configurable_entity->get_value());

      if (!isNew(configurable_entity_name)) continue;

      std::ostringstream oss;
      oss << "{";
      oss << "\"collection\" : \"" << collection_name << "\",";
      oss << "\"dbprovider\" : \"mongo\",";
      oss << "\"dataformat\" : \"gui\",";
      oss << "\"operation\" : \"findversions\",";
      oss << "\"filter\" : {";
      oss << "\"configurable_entity.name\" : " << configurable_entity_name;
      oss << "}";
      oss << "}";

      TRACE_(9, "StorageProvider::MongoDB::findConfigEntities() found document=<" << oss.str() << ">");

      returnCollection.emplace_back(oss.str());
    }
  }
  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, MongoDB>::addConfigToGlobalConfig(JsonData const& search_filter) {
  confirm(!search_filter.json_buffer.empty());
  auto returnCollection = std::list<JsonData>();

  TRACE_(8, "StorageProvider::MongoDB::addConfigToGlobalConfig() begin");
  TRACE_(8, "StorageProvider::MongoDB::addConfigToGlobalConfig() args data=<" << search_filter.json_buffer << ">");

  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, MongoDB>::listCollectionNames(JsonData const& search_filter) {
  confirm(!search_filter.json_buffer.empty());
  auto returnCollection = std::list<JsonData>();

  TRACE_(9, "StorageProvider::MongoDB::listCollectionNames() begin");
  TRACE_(9, "StorageProvider::MongoDB::listCollectionNames() args data=<" << search_filter.json_buffer << ">");

  auto filter = bsoncxx::builder::core(false);
  auto search_filter_doc = bbs::document{};
  filter.concatenate(search_filter_doc);

  auto collectionDescriptors = _provider->connection().list_collections(filter.view_document());

  for (auto const& collectionDescriptor : collectionDescriptors) {
    TRACE_(9, "StorageProvider::MongoDB::listCollectionNames() found collection=<"
                  << bsoncxx::to_json(collectionDescriptor) << ">");

    auto element_name = collectionDescriptor.find("name");

    if (element_name == collectionDescriptor.end())
      throw runtime_error("MongoDB") << "MongoDB returned invalid database collection descriptor.";

    auto string_value = element_name->get_value();

    auto collection_name = dequote(bsoncxx::to_json(string_value));

    if (collection_name == "system.indexes" || collection_name == system_metadata) continue;

    TRACE_(9,
           "StorageProvider::MongoDB::listCollectionNames() found "
           "collection_name=<"
               << collection_name << ">");

    std::ostringstream oss;
    oss << "{";
    oss << "\"collection\" : \"" << collection_name << "\",";
    oss << "\"dbprovider\" : \"mongo\",";
    oss << "\"dataformat\" : \"gui\",";
    oss << "\"operation\" : \"findversions\",";
    oss << "\"filter\" : {}";
    oss << "}";

    TRACE_(9, "StorageProvider::MongoDB::listCollectionNames() found document=<" << oss.str() << ">");

    returnCollection.emplace_back(oss.str());
  }
  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, MongoDB>::listDatabaseNames(JsonData const& search_filter) {
  confirm(!search_filter.json_buffer.empty());
  auto returnCollection = std::list<JsonData>();

  TRACE_(9, "StorageProvider::MongoDB::listDatabaseNames() begin");
  TRACE_(9, "StorageProvider::MongoDB::listDatabaseNames() args data=<" << search_filter.json_buffer << ">");

  mongocxx::cursor databaseDescriptors = _provider->list_databases();

  for (auto const& databaseDescriptor : databaseDescriptors) {
    TRACE_(9, "StorageProvider::MongoDB::listDatabaseNames() found databases=<" << bsoncxx::to_json(databaseDescriptor)
                                                                                << ">");

    auto element_name = databaseDescriptor.find("name");

    if (element_name == databaseDescriptor.end())
      throw runtime_error("MongoDB") << "MongoDB returned invalid database descriptor.";

    auto string_value = element_name->get_value();

    auto database_name = dequote(bsoncxx::to_json(string_value));

    if (database_name == "local") continue;

    TRACE_(9, "StorageProvider::MongoDB::listDatabaseNames() found database_name=<" << database_name << ">");

    std::ostringstream oss;
    oss << "{";
    oss << "\"database\" : \"" << database_name << "\",";
    oss << "\"dbprovider\" : \"mongo\",";
    oss << "\"dataformat\" : \"gui\",";
    oss << "\"filter\" : {}";
    oss << "}";

    TRACE_(9, "StorageProvider::MongoDB::listDatabaseNames() found document=<" << oss.str() << ">");

    returnCollection.emplace_back(oss.str());
  }
  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, MongoDB>::databaseMetadata(JsonData const& search_filter [[gnu::unused]]) {
  confirm(!search_filter.json_buffer.empty());

  std::ostringstream oss;
  oss << "{";
  oss << "\"collection\":\"" << system_metadata << "\",";
  oss << "\"filter\":{}";
  oss << "}";
  return load(JsonData{oss.str()});
}

namespace mongo {
namespace debug {
void enable() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TRACE_(0, "artdaq::database::mongo trace_enable");
}
}
}  // namespace mongo

}  // namespace database
}  // namespace artdaq
