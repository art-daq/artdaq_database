#include "artdaq-database/ConfigurationDB/common.h"

#include "artdaq-database/ConfigurationDB/dispatch_mongodb.h"
#include "artdaq-database/ConfigurationDB/options_operations.h"

#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/DataFormats/common/shared_literals.h"
#include "artdaq-database/JsonDocument/JSONDocument.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/StorageProviders/MongoDB/provider_mongodb.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "COMG:DpMon_C"

using namespace artdaq::database::configuration;

namespace DBI = artdaq::database::mongo;
namespace prov = artdaq::database::configuration::mongo;

using artdaq::database::basictypes::JsonData;
using artdaq::database::jsonutils::JSONDocumentBuilder;
using artdaq::database::jsonutils::JSONDocument;

void prov::store(LoadStoreOperation const& options, JsonData const& insert_payload) {
  assert(options.provider().compare(literal::provider::mongo) == 0);
  assert(options.operation().compare(literal::operation::store) == 0);

  if (options.operation().compare(literal::operation::store) != 0) {
    throw cet::exception("store_configuration") << "Wrong operation option; operation=<" << options.operation() << ">.";
  }

  if (options.provider().compare(literal::provider::mongo) != 0) {
    throw cet::exception("store_configuration") << "Wrong provider option; provider=<" << options.provider() << ">.";
  }

  TRACE_(15, "store: begin");

  auto config = DBI::DBConfig{};
  auto database = DBI::DB::create(config);
  auto provider = DBI::DBProvider<JsonData>::create(database);
  auto object_id = provider->store(insert_payload);

  TRACE_(15, "store: object_id=<" << object_id << ">");

  TRACE_(15, "store: end");
}

JsonData prov::load(LoadStoreOperation const& options, JsonData const& search_payload) {
  assert(options.provider().compare(literal::provider::mongo) == 0);
  assert(options.operation().compare(literal::operation::load) == 0);

  if (options.operation().compare(literal::operation::load) != 0) {
    throw cet::exception("load_configuration") << "Wrong operation option; operation=<" << options.operation() << ">.";
  }

  if (options.provider().compare(literal::provider::mongo) != 0) {
    throw cet::exception("load_configuration") << "Wrong provider option; provider=<" << options.provider() << ">.";
  }

  TRACE_(16, "load: begin");

  auto config = DBI::DBConfig{};
  auto database = DBI::DB::create(config);
  auto provider = DBI::DBProvider<JsonData>::create(database);

  auto collection = provider->load(search_payload);

  TRACE_(16, "load_configuration: "
                 << "Search returned " << collection.size() << " results.");

  if (collection.size() != 1) {
    throw cet::exception("load_configuration") << "Search returned " << collection.size() << " results.";
  }

  auto data = JsonData{collection.begin()->json_buffer};

  TRACE_(16, "load: end");

  return data;
}

JsonData prov::findGlobalConfigs(ManageConfigsOperation const& options, JsonData const& search_payload) {
  assert(options.provider().compare(literal::provider::mongo) == 0);
  assert(options.operation().compare(literal::operation::findconfigs) == 0);

  if (options.operation().compare(literal::operation::findconfigs) != 0) {
    throw cet::exception("operation_findconfigs") << "Wrong operation option; operation=<" << options.operation()
                                                  << ">.";
  }

  if (options.provider().compare(literal::provider::mongo) != 0) {
    throw cet::exception("operation_findconfigs") << "Wrong provider option; provider=<" << options.provider() << ">.";
  }

  TRACE_(17, "operation_findconfigs: begin");

  auto config = DBI::DBConfig{};
  auto database = DBI::DB::create(config);
  auto provider = DBI::DBProvider<JsonData>::create(database);

  auto global_configs = provider->findGlobalConfigs(search_payload);

  if (global_configs.empty()) {
    return {literal::empty_search_result};        
    //throw cet::exception("operation_findconfigs") << "No global configurations were found.";
  }

  auto needComma = bool{false};
  auto printComma = [&needComma]() {
    if (needComma) return ", ";
    needComma = true;
    return " ";
  };

  std::stringstream ss;

  auto seenValues = std::list<std::string>{};
  seenValues.push_back(literal::notprovided);

  auto isNew = [& v = seenValues](auto const& name) {
    assert(!name.empty());
    if (std::find(v.begin(), v.end(), name) != v.end()) return false;

    v.emplace_back(name);
    return true;
  };

  ss << "{ \"search\": [";

  for (auto const& global_config : global_configs) {
    auto doc = JSONDocument{global_config.json_buffer};

    auto global_config_name = JSONDocument::value(doc.findChild("filter").value());

    if (!isNew(global_config_name)) continue;

    ss << printComma() << "{";
    ss << "\"name\" :\"" << global_config_name << "\",";
    ss << "\"query\" :" << global_config.json_buffer;
    ss << "}";
  }

  ss << "] }";

  return {ss.str()};
}

JsonData prov::buildConfigSearchFilter(ManageConfigsOperation const& options, JsonData const& search_payload) {
  assert(options.provider().compare(literal::provider::mongo) == 0);
  assert(options.operation().compare(literal::operation::buildfilter) == 0);

  if (options.operation().compare(literal::operation::buildfilter) != 0) {
    throw cet::exception("operation_buildfilter") << "Wrong operation option; operation=<" << options.operation()
                                                  << ">.";
  }

  if (options.provider().compare(literal::provider::mongo) != 0) {
    throw cet::exception("operation_buildfilter") << "Wrong provider option; provider=<" << options.provider() << ">.";
  }

  TRACE_(18, "operation_buildfilter: begin");

  auto config = DBI::DBConfig{};
  auto database = DBI::DB::create(config);
  auto provider = DBI::DBProvider<JsonData>::create(database);

  auto search_filters = provider->buildConfigSearchFilter(search_payload);

  if (search_filters.empty()) {
    throw cet::exception("operation_buildfilter") << "No search filters were found.";
  }

  auto ex = std::regex(
      "\"configurations\\.name\"\\s*:\\s*\"((\\\\\"|[^\"])*)\"\\,\\s*\"configurable_entity\\.name\"\\s*:\\s*\"((\\\\\"|"
      "[^\"])*)\"");

  auto needComma = bool{false};
  auto printComma = [&needComma]() {
    if (needComma) return ", ";
    needComma = true;
    return " ";
  };

  std::stringstream ss;

  ss << "{ \"search\": [";

  for (auto const& search_filter : search_filters) {
    auto filter_json = JSONDocument{search_filter.json_buffer}.findChild("filter").value();

    auto results = std::smatch();

    if (!std::regex_search(filter_json, results, ex))
      throw cet::exception("operation_buildfilter") << "Unsupported filter string, no match";

    /*
        for (size_t i = 0; i < results.size(); ++i) {
          std::ssub_match sub_match = results[i];
          std::string piece = sub_match.str();
                     TRACE_(18, "operation_buildfilter: submatch*** " << i << ": " << piece << '\n');
        }
    */
    if (results.size() != 5)
      throw cet::exception("operation_buildfilter") << "Unsupported filter string, wrong result count";

    ss << printComma() << "{";
    ss << "\"name\" :\"" << results[1].str() << ":" << results[3].str() << "\",";
    ss << "\"query\" :" << search_filter.json_buffer;
    ss << "}";
  }

  ss << "] }";

  return {ss.str()};
}

JsonData prov::findConfigVersions(LoadStoreOperation const& options, JsonData const&) {
  assert(options.provider().compare(literal::provider::mongo) == 0);
  assert(options.operation().compare(literal::operation::findversions) == 0);

  if (options.operation().compare(literal::operation::findversions) != 0) {
    throw cet::exception("operation_findversions") << "Wrong operation option; operation=<" << options.operation()
                                                   << ">.";
  }

  if (options.provider().compare(literal::provider::mongo) != 0) {
    throw cet::exception("operation_findversions") << "Wrong provider option; provider=<" << options.provider() << ">.";
  }

  TRACE_(19, "operation_findversions: begin");

  auto config = DBI::DBConfig{};
  auto database = DBI::DB::create(config);
  auto provider = DBI::DBProvider<JsonData>::create(database);

  auto search_filter = options.to_JsonData();
  auto config_versions = provider->findConfigVersions(search_filter);

  if (config_versions.empty()) {
     return {literal::empty_search_result};    
    //throw cet::exception("operation_findversions") << "No configuration versions were found.";
  }

  auto ex = std::regex(
      "\"version\"\\s*:\\s*\"((\\\\\"|[^\"])*)\"\\,\\s*\"configurable_entity\\.name\"\\s*:\\s*\"((\\\\\"|"
      "[^\"])*)\"");

  auto needComma = bool{false};
  auto printComma = [&needComma]() {
    if (needComma) return ", ";
    needComma = true;
    return " ";
  };

  std::stringstream ss;

  ss << "{ \"search\": [";

  for (auto const& config_version : config_versions) {
    auto filter_json = JSONDocument{config_version.json_buffer}.findChild("filter").value();

    auto results = std::smatch();

    if (!std::regex_search(filter_json, results, ex))
      throw cet::exception("operation_findversions") << "Unsupported filter string, no match";

    /*
            for (size_t i = 0; i < results.size(); ++i) {
              std::ssub_match sub_match = results[i];
              std::string piece = sub_match.str();
                         TRACE_(18, "operation_findversions: submatch*** " << i << ": " << piece << '\n');
            }
    */
    if (results.size() != 5)
      throw cet::exception("operation_findversions") << "Unsupported filter string, wrong result count";

    ss << printComma() << "{";
    ss << "\"name\" :\"" << results[1].str() /*<< ":" << results[3].str()*/ << "\",";
    ss << "\"query\" :" << config_version.json_buffer;
    ss << "}";
  }

  ss << "] }";

  return {ss.str()};
}

JsonData prov::findConfigEntities(LoadStoreOperation const& options, JsonData const&) {
  assert(options.provider().compare(literal::provider::mongo) == 0);
  assert(options.operation().compare(literal::operation::findentities) == 0);

  if (options.operation().compare(literal::operation::findentities) != 0) {
    throw cet::exception("operation_findentities") << "Wrong operation option; operation=<" << options.operation()
                                                   << ">.";
  }

  if (options.provider().compare(literal::provider::mongo) != 0) {
    throw cet::exception("operation_findentities") << "Wrong provider option; provider=<" << options.provider() << ">.";
  }

  TRACE_(19, "operation_findentities: begin");

  auto config = DBI::DBConfig{};
  auto database = DBI::DB::create(config);
  auto provider = DBI::DBProvider<JsonData>::create(database);

  auto search_filter = options.to_JsonData();
  auto config_entities = provider->findConfigEntities(search_filter);

  if (config_entities.empty()) {
    return {literal::empty_search_result};        
   // throw cet::exception("operation_findentities") << "No configuration entities were found.";
  }

  auto ex = std::regex("\\s\"(configurable_entity\\.name)\"\\s:\\s\"((\\\\\"|[^\"])*)\"");

  auto needComma = bool{false};
  auto printComma = [&needComma]() {
    if (needComma) return ", ";
    needComma = true;
    return " ";
  };

  std::stringstream ss;

  ss << "{ \"search\": [";

  for (auto const& config_entity : config_entities) {
    auto filter_json = JSONDocument{config_entity.json_buffer}.findChild("filter").value();
    TRACE_(18, "operation_findentities: filter_json=<" << filter_json << '>');

    auto results = std::smatch();

    if (!std::regex_search(filter_json, results, ex))
      throw cet::exception("operation_findentities") << "Unsupported filter string, no match";

    /*

            for (size_t i = 0; i < results.size(); ++i) {
              std::ssub_match sub_match = results[i];
              std::string piece = sub_match.str();
                         TRACE_(18, "operation_findentities: submatch*** " << i << ": " << piece << '\n');
            }
    */

    if (results.size() != 4)
      throw cet::exception("operation_findentities") << "Unsupported filter string, wrong result count";

    ss << printComma() << "{";
    ss << "\"name\" :\"" << results[2].str() /*<< ":" << results[3].str()*/ << "\",";
    ss << "\"query\" :" << config_entity.json_buffer;
    ss << "}";
  }

  ss << "] }";

  return {ss.str()};
}

JsonData prov::addConfigToGlobalConfig(LoadStoreOperation const& options, JsonData const& search_payload) {
  assert(options.provider().compare(literal::provider::mongo) == 0);
  assert(options.operation().compare(literal::operation::addconfig) == 0);

  if (options.operation().compare(literal::operation::addconfig) != 0) {
    throw cet::exception("operation_addconfig") << "Wrong operation option; operation=<" << options.operation() << ">.";
  }

  if (options.provider().compare(literal::provider::mongo) != 0) {
    throw cet::exception("operation_addconfig") << "Wrong provider option; provider=<" << options.provider() << ">.";
  }

  TRACE_(20, "operation_addconfig: begin");

  auto new_options = options;
  new_options.operation(literal::operation::load);

  auto search =
      JsonData{"{\"filter\":" + search_payload.json_buffer + ", \"collection\":\"" + options.collectionName() + "\"}"};
  TRACE_(20, "operation_addconfig: args search_payload=<" << search.json_buffer << ">");

  auto document = mongo::load(new_options, search);
  auto json_document = JSONDocument{document.json_buffer};
  auto builder = JSONDocumentBuilder{json_document};
  auto globalConfiguration = JSONDocument{new_options.globalConfiguration_to_JsonData().json_buffer};

  builder.addToGlobalConfig(globalConfiguration);

  new_options.operation(literal::operation::store);

  auto update =
      JsonData{"{\"filter\":{\"$oid\":\"" + builder.extract().deleteChild("_id").value() + "\"},  \"document\":" +
               builder.extract().to_string() + ", \"collection\":\"" + options.collectionName() + "\"}"};

  mongo::store(new_options, update.json_buffer);

  new_options.operation(literal::operation::buildfilter);

  auto find_options = ManageConfigsOperation{literal::operation::addconfig};

  find_options.operation(literal::operation::buildfilter);
  find_options.dataFormat(options::data_format_t::gui);
  find_options.provider(literal::provider::mongo);
  find_options.globalConfiguration(new_options.globalConfiguration());

  return mongo::buildConfigSearchFilter(find_options, options.search_filter_to_JsonData().json_buffer);
}

void debug::enableDBOperationMongo() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);

  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TRACE_(0, "artdaq::database::configuration::Mongo trace_enable");
}
