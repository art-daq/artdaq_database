#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/StorageProviders/MongoDB/mongo_json.h"
#include "artdaq-database/StorageProviders/MongoDB/provider_mongodb.h"
#include "artdaq-database/StorageProviders/common.h"

#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
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
using artdaq::database::docrecord::JSONDocumentBuilder;
using artdaq::database::docrecord::JSONDocument;

namespace jsonliteral = artdaq::database::dataformats::literal;

//bsoncxx::types::value extract_value_from_document(bsoncxx::document::value const& document, std::string const& key);
namespace artdaq {
namespace database { 
  
template <>
template <>
std::list<JsonData> StorageProvider<JsonData, MongoDB>::findGlobalConfigs(JsonData const& search) {
  confirm(!search.json_buffer.empty());
  auto returnCollection = std::list<JsonData>();

  TRACE_(4, "MongoDB::findGlobalConfigs() begin");
  TRACE_(4, "MongoDB::findGlobalConfigs() args data=<" << search.json_buffer << ">");

  auto filter = bsoncxx::builder::core(false);

  auto search_filter_doc = bbs::document{};

  filter.concatenate(search_filter_doc);

  auto collectionDescriptors = _provider->connection().list_collections(filter.view_document());

  for (auto const& collectionDescriptor : collectionDescriptors) {
    TRACE_(4, "MongoDB::findGlobalConfigs() found collection=<" << bsoncxx::to_json(collectionDescriptor) << ">");

    auto element_name = collectionDescriptor.find("name");

    if (element_name == collectionDescriptor.end())
      throw runtime_error("MongoDB") << "MongoDB returned invalid database collection descriptor.";

    auto string_value = element_name->get_value();

    auto collection_name = dequote(bsoncxx::to_json(string_value));

    if (collection_name == "system.indexes" || collection_name == system_metadata) continue;

    TRACE_(4,
           "MongoDB::findGlobalConfigs() querying "
           "collection_name=<"
               << collection_name << ">");

    auto collection = _provider->connection().collection(collection_name);

    auto configuration_filter = bsoncxx::builder::core(false);
    auto bson_document = bsoncxx::from_json(search.json_buffer);
    configuration_filter.concatenate(bson_document.view());

    auto cursor = collection.distinct("configurations.name", configuration_filter.view_document());

    for (auto const& view : cursor) {
      TRACE_(4, "MongoDB::findGlobalConfigs() looping over cursor =<" << bsoncxx::to_json(view) << ">");

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
        TRACE_(4, "MongoDB::findGlobalConfigs() found document=<" << oss.str() << ">");

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

  TRACE_(5, "MongoDB::buildConfigSearchFilter() begin");
  TRACE_(5, "MongoDB::buildConfigSearchFilter() args data=<" << search.json_buffer << ">");

  auto filter = bsoncxx::builder::core(false);

  auto search_filter_doc = bbs::document{};

  filter.concatenate(search_filter_doc);

  auto collectionDescriptors = _provider->connection().list_collections(filter.view_document());

  for (auto const& collectionDescriptor : collectionDescriptors) {
    TRACE_(5, "MongoDB::buildConfigSearchFilter() found collection=<" << bsoncxx::to_json(collectionDescriptor) << ">");

    // auto view = collectionDescriptor.view();
    auto element_name = collectionDescriptor.find("name");

    if (element_name == collectionDescriptor.end())
      throw runtime_error("MongoDB") << "MongoDB returned invalid database collection descriptor.";

    auto string_value = element_name->get_value();

    auto collection_name = dequote(bsoncxx::to_json(string_value));

    if (collection_name == "system.indexes" || collection_name == system_metadata) continue;

    TRACE_(5,
           "MongoDB::buildConfigSearchFilter() querying "
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

    TRACE_(5, "MongoDB::buildConfigSearchFilter()  search_filter=<" << bsoncxx::to_json(stages.view()) << ">");

    auto cursor = collection.aggregate(stages);

    for (auto const& view : cursor) {
      TRACE_(5, "MongoDB::buildConfigSearchFilter()  value=<" << bsoncxx::to_json(view) << ">");

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

        TRACE_(4, "MongoDB::buildConfigSearchFilter() found document=<" << oss.str() << ">");

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

  TRACE_(7, "MongoDB::findConfigVersions() begin");
  TRACE_(7, "MongoDB::findConfigVersions() args data=<" << search_filter.json_buffer << ">");

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

  TRACE_(5, "MongoDB::findConfigVersions()  search_filter=<" << bsoncxx::to_json(stages.view()) << ">");

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

    TRACE_(4, "MongoDB::findConfigVersions() found document=<" << oss.str() << ">");

    returnCollection.emplace_back(oss.str());
  }

  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, MongoDB>::findConfigEntities(JsonData const& search) {
  confirm(!search.json_buffer.empty());
  auto returnCollection = std::list<JsonData>();

  TRACE_(9, "MongoDB::findConfigEntities() begin");
  TRACE_(9, "MongoDB::findConfigEntities() args data=<" << search.json_buffer << ">");

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
    TRACE_(9, "MongoDB::findConfigEntities() found collection=<" << bsoncxx::to_json(collectionDescriptor) << ">");

    auto element_name = collectionDescriptor.find("name");

    if (element_name == collectionDescriptor.end())
      throw runtime_error("MongoDB") << "MongoDB returned invalid database collection descriptor.";

    auto string_value = element_name->get_value();

    auto collection_name = dequote(bsoncxx::to_json(string_value));

    if (collection_name == "system.indexes" || collection_name == system_metadata) continue;

    TRACE_(9,
           "MongoDB::findConfigEntities() querying "
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

    TRACE_(9, "MongoDB::findConfigEntities()  search_filter=<" << bsoncxx::to_json(stages.view()) << ">");
    auto cursor = collection.aggregate(stages);

    auto seenValues = std::list<std::string>{};

    auto isNew = [& v = seenValues](auto const& name) {
      confirm(!name.empty());
      if (std::find(v.begin(), v.end(), name) != v.end()) return false;

      v.emplace_back(name);
      return true;
    };

    for (auto const& view : cursor) {
      TRACE_(9, "MongoDB::findConfigEntities()  value=<" << bsoncxx::to_json(view) << ">");

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

      TRACE_(9, "MongoDB::findConfigEntities() found document=<" << oss.str() << ">");

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

  TRACE_(8, "MongoDB::addConfigToGlobalConfig() begin");
  TRACE_(8, "MongoDB::addConfigToGlobalConfig() args data=<" << search_filter.json_buffer << ">");

  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, MongoDB>::listCollectionNames(JsonData const& search_filter) {
  confirm(!search_filter.json_buffer.empty());
  auto returnCollection = std::list<JsonData>();

  TRACE_(9, "MongoDB::listCollectionNames() begin");
  TRACE_(9, "MongoDB::listCollectionNames() args data=<" << search_filter.json_buffer << ">");

  auto filter = bsoncxx::builder::core(false);
  auto search_filter_doc = bbs::document{};
  filter.concatenate(search_filter_doc);

  auto collectionDescriptors = _provider->connection().list_collections(filter.view_document());

  for (auto const& collectionDescriptor : collectionDescriptors) {
    TRACE_(9, "MongoDB::listCollectionNames() found collection=<" << bsoncxx::to_json(collectionDescriptor) << ">");

    auto element_name = collectionDescriptor.find("name");

    if (element_name == collectionDescriptor.end())
      throw runtime_error("MongoDB") << "MongoDB returned invalid database collection descriptor.";

    auto string_value = element_name->get_value();

    auto collection_name = dequote(bsoncxx::to_json(string_value));

    if (collection_name == "system.indexes" || collection_name == system_metadata) continue;

    TRACE_(9,
           "MongoDB::listCollectionNames() found "
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

    TRACE_(9, "MongoDB::listCollectionNames() found document=<" << oss.str() << ">");

    returnCollection.emplace_back(oss.str());
  }
  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, MongoDB>::listDatabaseNames(JsonData const& search_filter) {
  confirm(!search_filter.json_buffer.empty());
  auto returnCollection = std::list<JsonData>();

  TRACE_(9, "MongoDB::listDatabaseNames() begin");
  TRACE_(9, "MongoDB::listDatabaseNames() args data=<" << search_filter.json_buffer << ">");

  mongocxx::cursor databaseDescriptors = _provider->list_databases();

  for (auto const& databaseDescriptor : databaseDescriptors) {
    TRACE_(9, "MongoDB::listDatabaseNames() found databases=<" << bsoncxx::to_json(databaseDescriptor) << ">");

    auto element_name = databaseDescriptor.find("name");

    if (element_name == databaseDescriptor.end())
      throw runtime_error("MongoDB") << "MongoDB returned invalid database descriptor.";

    auto string_value = element_name->get_value();

    auto database_name = dequote(bsoncxx::to_json(string_value));

    if (database_name == "local") continue;

    TRACE_(9, "MongoDB::listDatabaseNames() found database_name=<" << database_name << ">");

    std::ostringstream oss;
    oss << "{";
    oss << "\"database\" : \"" << database_name << "\",";
    oss << "\"dbprovider\" : \"mongo\",";
    oss << "\"dataformat\" : \"gui\",";
    oss << "\"filter\" : {}";
    oss << "}";

    TRACE_(9, "MongoDB::listDatabaseNames() found document=<" << oss.str() << ">");

    returnCollection.emplace_back(oss.str());
  }
  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, MongoDB>::databaseMetadata(JsonData const& search_filter[[gnu::unused]]) {
  confirm(!search_filter.json_buffer.empty());

  std::ostringstream oss;
  oss << "{";
  oss << "\"collection\":\"" << system_metadata << "\",";
  oss << "\"filter\":{}";
  oss << "}";
  return readConfiguration(JsonData{oss.str()});
}

namespace mongo {
namespace debug {
void enable() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  artdaq::database::mongo::debug::enableReadWrite();
  TRACE_(0, "artdaq::database::mongo trace_enable");
}
}
}  // namespace mongo

}  // namespace database
}  // namespace artdaq
