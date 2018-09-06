#include "artdaq-database/StorageProviders/MongoDB/provider_mongodb.h"
#include "artdaq-database/JsonDocument/JSONDocument.h"
#include "artdaq-database/StorageProviders/MongoDB/mongo_json.h"
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

#define TRACE_NAME "provider_mongodb.cpp"

using namespace artdaq::database;
namespace db = artdaq::database;

namespace bbs = bsoncxx::builder::stream;

using bsoncxx::builder::stream::document;

using artdaq::database::docrecord::JSONDocument;
using artdaq::database::mongo::MongoDB;

namespace jsonliteral = db::dataformats::literal;
namespace apiliteral = db::configapi::literal;

// bsoncxx::types::value extract_value_from_document(bsoncxx::document::value
// const& document, std::string const& key);
namespace artdaq {
namespace database {

namespace mongo {
JSONDocument rewrite_query_with_regex(JSONDocument const&, std::vector<std::string> const&);
}  // namespace mongo

template <>
template <>
std::vector<JSONDocument> StorageProvider<JSONDocument, MongoDB>::findConfigurations(JSONDocument const& search) {
  confirm(!search.empty());
  auto returnCollection = std::vector<JSONDocument>();

  using timestamp_t = unsigned long long;
  using ordered_timestamps_t = std::set<timestamp_t>;
  using config_timestamps_t = std::map<std::string, ordered_timestamps_t>;
  auto config_timestamps = config_timestamps_t();

  auto reverse_timestamp_cmp = [](const timestamp_t& a, const timestamp_t& b) { return a > b; };
  using timestamp_configs_t = std::multimap<timestamp_t, std::string, decltype(reverse_timestamp_cmp)>;

  auto timestamp_configs = timestamp_configs_t(reverse_timestamp_cmp);

  TLOG(14) << "MongoDB::findConfigurations() begin";
  TLOG(14) << "MongoDB::findConfigurations() args data=<" << search << ">";

  auto search_document = compat::from_json(search.to_string());

  auto extract_value = [&search_document](auto const& name) {
    auto view = search_document.view();
    auto element = view.find(name);

    if (element == view.end()) {
      throw runtime_error("MongoDB") << "Search JSONDocument is missing the \"" << name << "\" element.";
    }

    return element->get_value();
  };

  auto configuration_name_expected = db::dequote(compat::to_json(extract_value(apiliteral::filter::configurations)));

  if (configuration_name_expected.back() == '*') {
    configuration_name_expected.pop_back();
  }

  TLOG(15) << "MongoDB::findConfigurations()  configuration_name_expected=<" << configuration_name_expected << ">";
  auto fields = std::vector<std::string>{};
  fields.emplace_back(apiliteral::filter::configurations);

  auto regex_search = mongo::rewrite_query_with_regex(search, fields);

  TLOG(14) << "MongoDB::findConfigurations() regex_search data=<" << regex_search << ">";

  auto filter = bsoncxx::builder::core(false);

  auto query_payload_doc = bbs::document{};

  filter.concatenate(query_payload_doc);

  auto collectionDescriptors = _provider->connection().list_collections(filter.view_document());

  for (auto const& collectionDescriptor : collectionDescriptors) {
    TLOG(14) << "MongoDB::findConfigurations() found collection=<" << compat::to_json(collectionDescriptor) << ">";

    auto element_name = collectionDescriptor.find(jsonliteral::name);

    if (element_name == collectionDescriptor.end()) {
      throw runtime_error("MongoDB") << "MongoDB returned invalid database collection descriptor.";
    }

    auto string_value = element_name->get_value();

    auto collection_name = dequote(compat::to_json(string_value));

    if (collection_name == "system.indexes" || collection_name == system_metadata) {
      continue;
    }

    TLOG(14) << "MongoDB::findConfigurations() querying collection_name=<" << collection_name << ">";

    auto collection = _provider->connection().collection(collection_name);

    mongocxx::pipeline stages;

    auto match_stage = bsoncxx::builder::core(false);
    auto bson_document = compat::from_json(regex_search);
    match_stage.concatenate(bson_document.view());

    bbs::document project_stage;
    project_stage << "configurations"
                  << "$configurations";

    stages.match(match_stage.view_document()).project(project_stage.view());

    TLOG(14) << "MongoDB::findConfigurations() query_payload =<" << compat::to_json(stages.view_array()) << ">";

    auto cursor = collection.aggregate(stages);

    for (auto const& view : cursor) {
      TLOG(14) << "MongoDB::findConfigurations() looping over cursor =<" << compat::to_json(view) << ">";

      auto tmp_configurations = view.find(jsonliteral::configurations);
      if (tmp_configurations == collectionDescriptor.end()) {
        throw runtime_error("MongoDB") << "MongoDB returned invalid database search, \"configurations\" is missing.";
      }

      bsoncxx::array::view configurations{tmp_configurations->get_array().value};

      for (auto const& tmp_config : configurations) {
        bsoncxx::document::view configuration{tmp_config.get_document().value};

        auto tmp_config_name = configuration.find(jsonliteral::name);
        if (tmp_config_name == collectionDescriptor.end()) {
          throw runtime_error("MongoDB") << "MongoDB returned invalid database search, \"configurations.name\" is missing.";
        }

        auto name = db::replace_all(db::dequote(compat::to_json(tmp_config_name->get_value())), "\\/", "/");

        auto tmp_config_assigned = configuration.find(jsonliteral::assigned);
        if (tmp_config_assigned == collectionDescriptor.end()) {
          throw runtime_error("MongoDB") << "MongoDB returned invalid database search, \"configurations.assigned\" is missing.";
        }
        auto assigned = db::dequote(compat::to_json(tmp_config_assigned->get_value()));

        if (configuration_name_expected == name) {
          config_timestamps.clear();

          config_timestamps[name].insert(std::chrono::duration_cast<std::chrono::seconds>(db::to_timepoint(assigned).time_since_epoch()).count());

          goto exact_match;
        }

        if (!std::equal(configuration_name_expected.begin(), configuration_name_expected.end(), name.begin())) {
          continue;
        }

        config_timestamps[name].insert(std::chrono::duration_cast<std::chrono::seconds>(db::to_timepoint(assigned).time_since_epoch()).count());
      }
    }
  }

exact_match:

  for (auto const& cfg : config_timestamps) {
    timestamp_configs.emplace(*cfg.second.rbegin(), cfg.first);
  }

  // keys are sorted the reverse chronological order
  for (auto const& cfg : timestamp_configs) {
    std::ostringstream oss;
    oss << "{";
    // oss << db::quoted_(apiliteral::option::collection) <<":" << db::quoted_(collection_name) << ",";
    oss << db::quoted_(apiliteral::option::provider) << ":" << db::quoted_(apiliteral::provider::mongo) << ",";
    oss << db::quoted_(apiliteral::option::format) << ":" << db::quoted_(apiliteral::format::gui) << ",";
    oss << db::quoted_(apiliteral::option::operation) << ":" << db::quoted_(apiliteral::operation::confcomposition) << ",";
    oss << db::quoted_(apiliteral::option::searchfilter) << ":"
        << "{";
    oss << db::quoted_(apiliteral::filter::configurations) << ": " << db::quoted_(cfg.second);
    oss << "}";
    oss << "}";
    TLOG(14) << "MongoDB::findConfigurations() found document=<" << oss.str() << ">";

    returnCollection.emplace_back(oss.str());
  }

  return returnCollection;
}

template <>
template <>
std::vector<JSONDocument> StorageProvider<JSONDocument, MongoDB>::configurationComposition(JSONDocument const& search) {
  confirm(!search.empty());
  auto returnCollection = std::vector<JSONDocument>();

  TLOG(15) << "MongoDB::configurationComposition() begin";
  TLOG(15) << "MongoDB::configurationComposition() args data=<" << search << ">";

  auto filter = bsoncxx::builder::core(false);

  auto query_payload_doc = bbs::document{};

  filter.concatenate(query_payload_doc);

  auto collectionDescriptors = _provider->connection().list_collections(filter.view_document());

  for (auto const& collectionDescriptor : collectionDescriptors) {
    TLOG(15) << "MongoDB::configurationComposition() found collection=<" << compat::to_json(collectionDescriptor) << ">";

    // auto view = collectionDescriptor.view();
    auto element_name = collectionDescriptor.find(jsonliteral::name);

    if (element_name == collectionDescriptor.end()) {
      throw runtime_error("MongoDB") << "MongoDB returned invalid database collection descriptor.";
    }

    auto string_value = element_name->get_value();

    auto collection_name = dequote(compat::to_json(string_value));

    if (collection_name == "system.indexes" || collection_name == system_metadata) {
      continue;
    }

    TLOG(15) << "MongoDB::configurationComposition() querying collection_name=<" << collection_name << ">";

    auto collection = _provider->connection().collection(collection_name);
    auto bson_document = compat::from_json(search.to_string());

    auto extract_value = [&bson_document](auto const& name) {
      auto view = bson_document.view();
      auto element = view.find(name);

      if (element == view.end()) {
        throw runtime_error("MongoDB") << "Search JSONDocument is missing the \"" << name << "\" element.";
      }

      return element->get_value();
    };

    auto configuration_name_expected = compat::to_json(extract_value(apiliteral::filter::configurations));
    TLOG(15) << "MongoDB::configurationComposition()  configuration_name_expected=<" << configuration_name_expected << ">";

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

    TLOG(15) << "MongoDB::configurationComposition()  query_payload=<" << compat::to_json(stages.view_array()) << ">";

    auto cursor = collection.aggregate(stages);

    for (auto const& view : cursor) {
      TLOG(15) << "MongoDB::configurationComposition()  value=<" << compat::to_json(view) << ">";

      auto configurations_value = view.find("configurations");
      auto entities_value = view.find("entities");

      if (configurations_value == collectionDescriptor.end() || entities_value == collectionDescriptor.end()) {
        throw runtime_error("MongoDB") << "MongoDB returned invalid database search.";
      }

      auto configurations = configurations_value->get_array();
      auto entities = entities_value->get_array();

      for (auto const& configuration : configurations.value) {
        for (auto const& entity : entities.value) {
          auto configuration_name = compat::to_json(configuration.get_value());
          auto entity_name = compat::to_json(entity.get_value());

          if (configuration_name != configuration_name_expected) {
            continue;
          }

          std::ostringstream oss;
          oss << "{";
          oss << db::quoted_(apiliteral::option::collection) << ":" << db::quoted_(collection_name) << ",";
          oss << db::quoted_(apiliteral::option::provider) << ":" << db::quoted_(apiliteral::provider::mongo) << ",";
          oss << db::quoted_(apiliteral::option::format) << ":" << db::quoted_(apiliteral::format::gui) << ",";
          oss << db::quoted_(apiliteral::option::operation) << ":" << db::quoted_(apiliteral::operation::readdocument) << ",";
          oss << db::quoted_(apiliteral::option::searchfilter) << ":"
              << "{";
          oss << db::quoted_(apiliteral::filter::configurations) << ": " << configuration_name;
          oss << "," << db::quoted_(apiliteral::filter::entities) << ": " << entity_name;
          oss << "}";
          oss << "}";

          TLOG(14) << "MongoDB::configurationComposition() found document=<" << oss.str() << ">";

          returnCollection.emplace_back(oss.str());
        }
      }
    }
  }

  if (returnCollection.empty()) {
    TLOG(14) << "MongoDB::configurationComposition() No data found for json=<" << search << ">";
  }

  return returnCollection;
}

template <>
template <>
std::vector<JSONDocument> StorageProvider<JSONDocument, MongoDB>::findVersions(JSONDocument const& query_payload) {
  confirm(!query_payload.empty());
  auto returnCollection = std::vector<JSONDocument>();

  TLOG(15) << "MongoDB::findVersions() begin";
  TLOG(15) << "MongoDB::findVersions() args data=<" << query_payload << ">";

  auto bson_document = compat::from_json(query_payload);

  auto extract_value = [&bson_document](auto const& name) {
    auto view = bson_document.view();
    auto element = view.find(name);

    if (element == view.end()) {
      throw runtime_error("MongoDB") << "Search JSONDocument is missing the \"" << name << "\" element.";
    }

    return element->get_value();
  };

  auto collection_name = dequote(compat::to_json(extract_value(apiliteral::option::collection)));

  auto collection = _provider->connection().collection(collection_name);

  auto search = JSONDocument{compat::to_json(extract_value(apiliteral::option::searchfilter))};

  auto fields = std::vector<std::string>{};
  fields.emplace_back(apiliteral::filter::entities);
  auto regex_search = mongo::rewrite_query_with_regex(search, fields);
  TLOG(15) << "MongoDB::findVersions() args regex_search=<" << regex_search << ">";

  mongocxx::pipeline stages;
  bbs::document project_stage;
  auto match_stage = bsoncxx::builder::core(false);
  auto bson_search = compat::from_json(regex_search);
  match_stage.concatenate(bson_search.view());

  project_stage << "_id" << 0 << "version"
                << "$version"
                << "entities"
                << "$entities.name";

  stages.match(match_stage.view_document()).project(project_stage.view());

  TLOG(15) << "MongoDB::findVersions()  query_payload=<" << compat::to_json(stages.view_array()) << ">";

  auto cursor = collection.aggregate(stages);

  for (auto const& view : cursor) {
    auto version = view.find("version");
    auto version_name = compat::to_json(version->get_value());

    auto entities = view.find("entities")->get_array();

    for (auto const& entity : entities.value) {
      auto entity_name = compat::to_json(entity.get_value());

      std::ostringstream oss;
      oss << "{";
      oss << db::quoted_(apiliteral::option::collection) << ":" << db::quoted_(collection_name) << ",";
      oss << db::quoted_(apiliteral::option::provider) << ":" << db::quoted_(apiliteral::provider::mongo) << ",";
      oss << db::quoted_(apiliteral::option::format) << ":" << db::quoted_(apiliteral::format::gui) << ",";
      oss << db::quoted_(apiliteral::option::operation) << ":" << db::quoted_(apiliteral::operation::readdocument) << ",";
      oss << db::quoted_(apiliteral::option::searchfilter) << ":"
          << "{";
      oss << db::quoted_(apiliteral::filter::version) << ": " << version_name;
      oss << "," << db::quoted_(apiliteral::filter::entities) << ": " << entity_name;
      oss << "}";
      oss << "}";

      TLOG(14) << "MongoDB::findVersions() found document=<" << oss.str() << ">";

      returnCollection.emplace_back(oss.str());
    }
  }

  return returnCollection;
}

template <>
template <>
std::vector<JSONDocument> StorageProvider<JSONDocument, MongoDB>::findEntities(JSONDocument const& search) {
  confirm(!search.empty());
  auto returnCollection = std::vector<JSONDocument>();

  TLOG(19) << "MongoDB::findEntities() begin";
  TLOG(19) << "MongoDB::findEntities() args data=<" << search << ">";

  auto bson_document = compat::from_json(search);

  auto extract_value = [&bson_document](auto const& name) {
    auto view = bson_document.view();
    auto element = view.find(name);

    if (element == view.end()) {
      throw runtime_error("MongoDB") << "Search JSONDocument is missing the \"" << name << "\" element.";
    }

    return element->get_value();
  };

  auto filter = bsoncxx::builder::core(false);
  auto query_payload_doc = bbs::document{};
  filter.concatenate(query_payload_doc);

  auto collectionDescriptors = _provider->connection().list_collections(filter.view_document());

  for (auto const& collectionDescriptor : collectionDescriptors) {
    TLOG(19) << "MongoDB::findEntities() found collection=<" << compat::to_json(collectionDescriptor) << ">";

    auto element_name = collectionDescriptor.find(jsonliteral::name);

    if (element_name == collectionDescriptor.end()) {
      throw runtime_error("MongoDB") << "MongoDB returned invalid database collection descriptor.";
    }

    auto string_value = element_name->get_value();

    auto collection_name = dequote(compat::to_json(string_value));

    if (collection_name == "system.indexes" || collection_name == system_metadata) {
      continue;
    }

    TLOG(19) << "MongoDB::findEntities() querying collection_name=<" << collection_name << ">";

    auto collection = _provider->connection().collection(collection_name);
    auto bson_document = compat::from_json(search);

    mongocxx::pipeline stages;
    bbs::document project_stage;
    auto match_stage = bsoncxx::builder::core(false);

    match_stage.concatenate(extract_value(apiliteral::option::searchfilter).get_document().value);

    project_stage << "_id" << 0 << "entities"
                  << "$entities.name";

    stages.match(match_stage.view_document()).project(project_stage.view());

    TLOG(19) << "MongoDB::findEntities()  query_payload=<" << compat::to_json(stages.view_array()) << ">";
    auto cursor = collection.aggregate(stages);

    auto seenValues = std::list<std::string>{};

    auto isNew = [& v = seenValues](auto const& name) {
      confirm(!name.empty());
      if (std::find(v.begin(), v.end(), name) != v.end()) {
        return false;
      }

      v.emplace_back(name);
      return true;
    };

    for (auto const& view : cursor) {
      TLOG(19) << "MongoDB::findEntities()  value=<" << compat::to_json(view) << ">";

      auto entities = view.find("entities")->get_array();

      for (auto const& entity : entities.value) {
        auto entity_name = compat::to_json(entity.get_value());

        if (!isNew(entity_name)) {
          continue;
        }

        std::ostringstream oss;
        oss << "{";
        oss << db::quoted_(apiliteral::option::collection) << ":" << db::quoted_(collection_name) << ",";
        oss << db::quoted_(apiliteral::option::provider) << ":" << db::quoted_(apiliteral::provider::mongo) << ",";
        oss << db::quoted_(apiliteral::option::format) << ":" << db::quoted_(apiliteral::format::gui) << ",";
        oss << db::quoted_(apiliteral::option::operation) << ":" << db::quoted_(apiliteral::operation::findversions) << ",";
        oss << db::quoted_(apiliteral::option::searchfilter) << ":"
            << "{";
        oss << db::quoted_(apiliteral::filter::entities) << ": " << entity_name;
        oss << "}";
        oss << "}";

        TLOG(19) << "MongoDB::findEntities() found document=<" << oss.str() << ">";

        returnCollection.emplace_back(oss.str());
      }
    }
  }
  return returnCollection;
}

template <>
template <>
std::vector<JSONDocument> StorageProvider<JSONDocument, MongoDB>::addConfiguration(JSONDocument const& query_payload) {
  confirm(!query_payload.empty());
  auto returnCollection = std::vector<JSONDocument>();

  TLOG(15) << "MongoDB::addConfiguration() begin";
  TLOG(15) << "MongoDB::addConfiguration() args data=<" << query_payload << ">";

  return returnCollection;
}

template <>
template <>
std::vector<JSONDocument> StorageProvider<JSONDocument, MongoDB>::listCollections(JSONDocument const& query_payload) {
  confirm(!query_payload.empty());
  auto returnCollection = std::vector<JSONDocument>();

  TLOG(19) << "MongoDB::listCollections() begin";
  TLOG(19) << "MongoDB::listCollections() args data=<" << query_payload << ">";

  auto filter = bsoncxx::builder::core(false);
  auto query_payload_doc = bbs::document{};
  filter.concatenate(query_payload_doc);

  auto collectionDescriptors = _provider->connection().list_collections(filter.view_document());

  for (auto const& collectionDescriptor : collectionDescriptors) {
    TLOG(19) << "MongoDB::listCollections() found collection=<" << compat::to_json(collectionDescriptor) << ">";

    auto element_name = collectionDescriptor.find(jsonliteral::name);

    if (element_name == collectionDescriptor.end()) {
      throw runtime_error("MongoDB") << "MongoDB returned invalid database collection descriptor.";
    }

    auto string_value = element_name->get_value();

    auto collection_name = dequote(compat::to_json(string_value));

    if (collection_name == "system.indexes" || collection_name == system_metadata) {
      continue;
    }

    TLOG(19) << "MongoDB::listCollections() found collection_name=<" << collection_name << ">";

    std::ostringstream oss;
    oss << "{";
    oss << db::quoted_(apiliteral::option::collection) << ":" << db::quoted_(collection_name) << ",";
    oss << db::quoted_(apiliteral::option::provider) << ":" << db::quoted_(apiliteral::provider::mongo) << ",";
    oss << db::quoted_(apiliteral::option::format) << ":" << db::quoted_(apiliteral::format::gui) << ",";
    oss << db::quoted_(apiliteral::option::operation) << ":" << db::quoted_(apiliteral::operation::findversions) << ",";
    oss << db::quoted_(apiliteral::option::searchfilter) << ":" << apiliteral::empty_json;
    oss << "}";

    TLOG(19) << "MongoDB::listCollections() found document=<" << oss.str() << ">";

    returnCollection.emplace_back(oss.str());
  }
  return returnCollection;
}

template <>
template <>
std::vector<JSONDocument> StorageProvider<JSONDocument, MongoDB>::listDatabases(JSONDocument const& query_payload) {
  confirm(!query_payload.empty());
  auto returnCollection = std::vector<JSONDocument>();

  TLOG(19) << "MongoDB::listDatabases() begin";
  TLOG(19) << "MongoDB::listDatabases() args data=<" << query_payload << ">";

  mongocxx::cursor databaseDescriptors = _provider->list_databases();

  for (auto const& databaseDescriptor : databaseDescriptors) {
    TLOG(19) << "MongoDB::listDatabases() found databases=<" << compat::to_json(databaseDescriptor) << ">";

    auto element_name = databaseDescriptor.find(jsonliteral::name);

    if (element_name == databaseDescriptor.end()) {
      throw runtime_error("MongoDB") << "MongoDB returned invalid database descriptor.";
    }

    auto string_value = element_name->get_value();

    auto database_name = dequote(compat::to_json(string_value));

    if (database_name == "local" || database_name == "admin") {
      continue;
    }

    TLOG(19) << "MongoDB::listDatabases() found database_name=<" << database_name << ">";

    std::ostringstream oss;
    oss << "{";
    oss << db::quoted_(apiliteral::database) << ":" << db::quoted_(database_name) << ",";
    oss << db::quoted_(apiliteral::option::provider) << ":" << db::quoted_(apiliteral::provider::mongo) << ",";
    oss << db::quoted_(apiliteral::option::format) << ":" << db::quoted_(apiliteral::format::gui) << ",";
    oss << db::quoted_(apiliteral::option::searchfilter) << ":" << apiliteral::empty_json;
    oss << "}";

    TLOG(19) << "MongoDB::listDatabases() found document=<" << oss.str() << ">";

    returnCollection.emplace_back(oss.str());
  }
  return returnCollection;
}

template <>
template <>
std::vector<JSONDocument> StorageProvider<JSONDocument, MongoDB>::databaseMetadata(JSONDocument const& query_payload[[gnu::unused]]) {
  confirm(!query_payload.empty());

  std::ostringstream oss;
  oss << "{";
  oss << db::quoted_(apiliteral::option::collection) << ":" << db::quoted_(system_metadata) << ",";
  oss << db::quoted_(apiliteral::option::searchfilter) << ":" << apiliteral::empty_json;
  oss << "}";
  return readDocument(JSONDocument{oss.str()});
}

namespace mongo {
namespace debug {
void enable() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  artdaq::database::mongo::debug::ReadWrite();
  TLOG(10) << "artdaq::database::mongo trace_enable";
}
}  // namespace debug
}  // namespace mongo

}  // namespace database
}  // namespace artdaq
