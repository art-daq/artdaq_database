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

// bsoncxx::types::value extract_value_from_document(bsoncxx::document::value const& document, std::string const& key);
namespace artdaq {
namespace database {

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, MongoDB>::findConfigurations(JsonData const& search) {
  confirm(!search.json_buffer.empty());
  auto returnCollection = std::list<JsonData>();

  TRACE_(4, "MongoDB::findConfigurations() begin");
  TRACE_(4, "MongoDB::findConfigurations() args data=<" << search.json_buffer << ">");

  auto filter = bsoncxx::builder::core(false);

  auto query_payload_doc = bbs::document{};

  filter.concatenate(query_payload_doc);

  auto collectionDescriptors = _provider->connection().list_collections(filter.view_document());

  for (auto const& collectionDescriptor : collectionDescriptors) {
    TRACE_(4, "MongoDB::findConfigurations() found collection=<" << bsoncxx::to_json(collectionDescriptor) << ">");

    auto element_name = collectionDescriptor.find("name");

    if (element_name == collectionDescriptor.end())
      throw runtime_error("MongoDB") << "MongoDB returned invalid database collection descriptor.";

    auto string_value = element_name->get_value();

    auto collection_name = dequote(bsoncxx::to_json(string_value));

    if (collection_name == "system.indexes" || collection_name == system_metadata) continue;

    TRACE_(4,
           "MongoDB::findConfigurations() querying "
           "collection_name=<"
               << collection_name << ">");

    auto collection = _provider->connection().collection(collection_name);

    auto configuration_filter = bsoncxx::builder::core(false);
    auto bson_document = bsoncxx::from_json(search.json_buffer);
    configuration_filter.concatenate(bson_document.view());

    auto cursor = collection.distinct("configurations.name", configuration_filter.view_document());

    for (auto const& view : cursor) {
      TRACE_(4, "MongoDB::findConfigurations() looping over cursor =<" << bsoncxx::to_json(view) << ">");

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
        TRACE_(4, "MongoDB::findConfigurations() found document=<" << oss.str() << ">");

        returnCollection.emplace_back(oss.str());
      }
    }
  }

  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, MongoDB>::configurationComposition(JsonData const& search) {
  confirm(!search.json_buffer.empty());
  auto returnCollection = std::list<JsonData>();

  TRACE_(5, "MongoDB::configurationComposition() begin");
  TRACE_(5, "MongoDB::configurationComposition() args data=<" << search.json_buffer << ">");

  auto filter = bsoncxx::builder::core(false);

  auto query_payload_doc = bbs::document{};

  filter.concatenate(query_payload_doc);

  auto collectionDescriptors = _provider->connection().list_collections(filter.view_document());

  for (auto const& collectionDescriptor : collectionDescriptors) {
    TRACE_(5, "MongoDB::configurationComposition() found collection=<" << bsoncxx::to_json(collectionDescriptor)
                                                                       << ">");

    // auto view = collectionDescriptor.view();
    auto element_name = collectionDescriptor.find("name");

    if (element_name == collectionDescriptor.end())
      throw runtime_error("MongoDB") << "MongoDB returned invalid database collection descriptor.";

    auto string_value = element_name->get_value();

    auto collection_name = dequote(bsoncxx::to_json(string_value));

    if (collection_name == "system.indexes" || collection_name == system_metadata) continue;

    TRACE_(5,
           "MongoDB::configurationComposition() querying "
           "collection_name=<"
               << collection_name << ">");

    auto collection = _provider->connection().collection(collection_name);
    auto bson_document = bsoncxx::from_json(search.json_buffer);

    auto extract_value = [&bson_document](auto const& name) {
      auto view = bson_document.view();
      auto element = view.find(name);

      if (element == view.end())
        throw runtime_error("MongoDB") << "Search JsonData is missing the \"" << name << "\" element.";

      return element->get_value();
    };

    auto configuration_name_expected = bsoncxx::to_json(extract_value("configurations.name"));
    TRACE_(5, "MongoDB::configurationComposition()  configuration_name_expected=<" << configuration_name_expected << ">");

    mongocxx::pipeline stages;
    bbs::document project_stage;
    auto match_stage = bsoncxx::builder::core(false);

    match_stage.concatenate(bson_document.view());

    project_stage << "_id" << 0 << "configurations"
                  << "$configurations.name"
                  << "entities"
                  << "$entities.name";

    bbs::document sort_stage;
    sort_stage << "configurations.name" << 1 << "entities.name" << 1;

    stages.match(match_stage.view_document()).project(project_stage.view());  //.sort(sort_stage.view());

    TRACE_(5, "MongoDB::configurationComposition()  query_payload=<" << bsoncxx::to_json(stages.view()) << ">");

    auto cursor = collection.aggregate(stages);

    for (auto const& view : cursor) {
      TRACE_(5, "MongoDB::configurationComposition()  value=<" << bsoncxx::to_json(view) << ">");

      auto configurations_value = view.find("configurations");
      auto entities_value = view.find("entities");

      if (configurations_value == collectionDescriptor.end() || entities_value == collectionDescriptor.end())
        throw runtime_error("MongoDB") << "MongoDB returned invalid database search.";

      auto configurations = configurations_value->get_array();
      auto entities = entities_value->get_array();

      for (auto const& configuration : configurations.value) {
        for (auto const& entity : entities.value) {
          auto configuration_name = bsoncxx::to_json(configuration.get_value());
          auto entity_name = bsoncxx::to_json(entity.get_value());

          if (configuration_name != configuration_name_expected) continue;

          std::ostringstream oss;
          oss << "{";
          oss << "\"collection\" : \"" << collection_name << "\",";
          oss << "\"dbprovider\" : \"mongo\",";
          oss << "\"dataformat\" : \"gui\",";
          oss << "\"operation\" : \"load\",";
          oss << "\"filter\" : {";
          oss << "\"configurations.name\" : " << configuration_name;
          oss << ", \"entities.name\" : " << entity_name;
          oss << "}";
          oss << "}";

          TRACE_(4, "MongoDB::configurationComposition() found document=<" << oss.str() << ">");

          returnCollection.emplace_back(oss.str());
        }
      }
    }
  }

  if (returnCollection.empty()) {
    TRACE_(4, "MongoDB::configurationComposition() No data found for json=<" << search.json_buffer << ">");
  }

  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, MongoDB>::findVersions(JsonData const& query_payload) {
  confirm(!query_payload.json_buffer.empty());
  auto returnCollection = std::list<JsonData>();

  TRACE_(7, "MongoDB::findVersions() begin");
  TRACE_(7, "MongoDB::findVersions() args data=<" << query_payload.json_buffer << ">");

  auto bson_document = bsoncxx::from_json(query_payload.json_buffer);

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
                << "entities"
                << "$entities.name";

  stages.match(match_stage.view_document()).project(project_stage.view());

  TRACE_(5, "MongoDB::findVersions()  query_payload=<" << bsoncxx::to_json(stages.view()) << ">");

  auto cursor = collection.aggregate(stages);

  for (auto const& view : cursor) {
    auto version = view.find("version");
    auto version_name = bsoncxx::to_json(version->get_value());

    auto entities = view.find("entities")->get_array();

    for (auto const& entity : entities.value) {
      auto entity_name = bsoncxx::to_json(entity.get_value());

      std::ostringstream oss;
      oss << "{";
      oss << "\"collection\" : \"" << collection_name << "\",";
      oss << "\"dbprovider\" : \"mongo\",";
      oss << "\"dataformat\" : \"gui\",";
      oss << "\"operation\" : \"load\",";
      oss << "\"filter\" : {";
      oss << "\"version\" : " << version_name;
      oss << ", \"entities.name\" : " << entity_name;
      oss << "}";
      oss << "}";

      TRACE_(4, "MongoDB::findVersions() found document=<" << oss.str() << ">");

      returnCollection.emplace_back(oss.str());
    }
  }

  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, MongoDB>::findEntities(JsonData const& search) {
  confirm(!search.json_buffer.empty());
  auto returnCollection = std::list<JsonData>();

  TRACE_(9, "MongoDB::findEntities() begin");
  TRACE_(9, "MongoDB::findEntities() args data=<" << search.json_buffer << ">");

  auto bson_document = bsoncxx::from_json(search.json_buffer);

  auto extract_value = [&bson_document](auto const& name) {
    auto view = bson_document.view();
    auto element = view.find(name);

    if (element == view.end())
      throw runtime_error("MongoDB") << "Search JsonData is missing the \"" << name << "\" element.";

    return element->get_value();
  };

  auto filter = bsoncxx::builder::core(false);
  auto query_payload_doc = bbs::document{};
  filter.concatenate(query_payload_doc);

  auto collectionDescriptors = _provider->connection().list_collections(filter.view_document());

  for (auto const& collectionDescriptor : collectionDescriptors) {
    TRACE_(9, "MongoDB::findEntities() found collection=<" << bsoncxx::to_json(collectionDescriptor) << ">");

    auto element_name = collectionDescriptor.find("name");

    if (element_name == collectionDescriptor.end())
      throw runtime_error("MongoDB") << "MongoDB returned invalid database collection descriptor.";

    auto string_value = element_name->get_value();

    auto collection_name = dequote(bsoncxx::to_json(string_value));

    if (collection_name == "system.indexes" || collection_name == system_metadata) continue;

    TRACE_(9,
           "MongoDB::findEntities() querying "
           "collection_name=<"
               << collection_name << ">");

    auto collection = _provider->connection().collection(collection_name);
    auto bson_document = bsoncxx::from_json(search.json_buffer);

    mongocxx::pipeline stages;
    bbs::document project_stage;
    auto match_stage = bsoncxx::builder::core(false);

    match_stage.concatenate(extract_value("filter").get_document().value);

    project_stage << "_id" << 0 << "entities"
                  << "$entities.name";

    stages.match(match_stage.view_document()).project(project_stage.view());

    TRACE_(9, "MongoDB::findEntities()  query_payload=<" << bsoncxx::to_json(stages.view()) << ">");
    auto cursor = collection.aggregate(stages);

    auto seenValues = std::list<std::string>{};

    auto isNew = [& v = seenValues](auto const& name) {
      confirm(!name.empty());
      if (std::find(v.begin(), v.end(), name) != v.end()) return false;

      v.emplace_back(name);
      return true;
    };

    for (auto const& view : cursor) {
      TRACE_(9, "MongoDB::findEntities()  value=<" << bsoncxx::to_json(view) << ">");

      auto entities = view.find("entities")->get_array();

      for (auto const& entity : entities.value) {
        auto entity_name = bsoncxx::to_json(entity.get_value());

        if (!isNew(entity_name)) continue;

        std::ostringstream oss;
        oss << "{";
        oss << "\"collection\" : \"" << collection_name << "\",";
        oss << "\"dbprovider\" : \"mongo\",";
        oss << "\"dataformat\" : \"gui\",";
        oss << "\"operation\" : \"findversions\",";
        oss << "\"filter\" : {";
        oss << "\"entities.name\" : " << entity_name;
        oss << "}";
        oss << "}";

        TRACE_(9, "MongoDB::findEntities() found document=<" << oss.str() << ">");

        returnCollection.emplace_back(oss.str());
      }
    }
  }
  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, MongoDB>::addConfiguration(JsonData const& query_payload) {
  confirm(!query_payload.json_buffer.empty());
  auto returnCollection = std::list<JsonData>();

  TRACE_(8, "MongoDB::addConfiguration() begin");
  TRACE_(8, "MongoDB::addConfiguration() args data=<" << query_payload.json_buffer << ">");

  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, MongoDB>::listCollections(JsonData const& query_payload) {
  confirm(!query_payload.json_buffer.empty());
  auto returnCollection = std::list<JsonData>();

  TRACE_(9, "MongoDB::listCollections() begin");
  TRACE_(9, "MongoDB::listCollections() args data=<" << query_payload.json_buffer << ">");

  auto filter = bsoncxx::builder::core(false);
  auto query_payload_doc = bbs::document{};
  filter.concatenate(query_payload_doc);

  auto collectionDescriptors = _provider->connection().list_collections(filter.view_document());

  for (auto const& collectionDescriptor : collectionDescriptors) {
    TRACE_(9, "MongoDB::listCollections() found collection=<" << bsoncxx::to_json(collectionDescriptor) << ">");

    auto element_name = collectionDescriptor.find("name");

    if (element_name == collectionDescriptor.end())
      throw runtime_error("MongoDB") << "MongoDB returned invalid database collection descriptor.";

    auto string_value = element_name->get_value();

    auto collection_name = dequote(bsoncxx::to_json(string_value));

    if (collection_name == "system.indexes" || collection_name == system_metadata) continue;

    TRACE_(9,
           "MongoDB::listCollections() found "
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

    TRACE_(9, "MongoDB::listCollections() found document=<" << oss.str() << ">");

    returnCollection.emplace_back(oss.str());
  }
  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, MongoDB>::listDatabases(JsonData const& query_payload) {
  confirm(!query_payload.json_buffer.empty());
  auto returnCollection = std::list<JsonData>();

  TRACE_(9, "MongoDB::listDatabases() begin");
  TRACE_(9, "MongoDB::listDatabases() args data=<" << query_payload.json_buffer << ">");

  mongocxx::cursor databaseDescriptors = _provider->list_databases();

  for (auto const& databaseDescriptor : databaseDescriptors) {
    TRACE_(9, "MongoDB::listDatabases() found databases=<" << bsoncxx::to_json(databaseDescriptor) << ">");

    auto element_name = databaseDescriptor.find("name");

    if (element_name == databaseDescriptor.end())
      throw runtime_error("MongoDB") << "MongoDB returned invalid database descriptor.";

    auto string_value = element_name->get_value();

    auto database_name = dequote(bsoncxx::to_json(string_value));

    if (database_name == "local") continue;

    TRACE_(9, "MongoDB::listDatabases() found database_name=<" << database_name << ">");

    std::ostringstream oss;
    oss << "{";
    oss << "\"database\" : \"" << database_name << "\",";
    oss << "\"dbprovider\" : \"mongo\",";
    oss << "\"dataformat\" : \"gui\",";
    oss << "\"filter\" : {}";
    oss << "}";

    TRACE_(9, "MongoDB::listDatabases() found document=<" << oss.str() << ">");

    returnCollection.emplace_back(oss.str());
  }
  return returnCollection;
}

template <>
template <>
std::list<JsonData> StorageProvider<JsonData, MongoDB>::databaseMetadata(JsonData const& query_payload[[gnu::unused]]) {
  confirm(!query_payload.json_buffer.empty());

  std::ostringstream oss;
  oss << "{";
  oss << "\"collection\":\"" << system_metadata << "\",";
  oss << "\"filter\":{}";
  oss << "}";
  return readDocument(JsonData{oss.str()});
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
