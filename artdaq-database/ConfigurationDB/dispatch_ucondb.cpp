#include "artdaq-database/ConfigurationDB/common.h"

#include "artdaq-database/ConfigurationDB/dispatch_ucondb.h"
#include "artdaq-database/ConfigurationDB/options_operations.h"

#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/DataFormats/shared_literals.h"
#include "artdaq-database/JsonDocument/JSONDocument.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/StorageProviders/UconDB/provider_ucondb.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "COMG:DpUcnd_C"

using namespace artdaq::database::configuration;
namespace cf = artdaq::database::configuration;

namespace DBI = artdaq::database::ucon;
namespace prov = artdaq::database::configuration::ucon;

using artdaq::database::basictypes::JsonData;
using artdaq::database::docrecord::JSONDocumentBuilder;
using artdaq::database::docrecord::JSONDocument;

void prov::writeDocument(ManageDocumentOperation const& options, JsonData const& insert_payload) {
  confirm(options.provider().compare(apiliteral::provider::ucon) == 0);
  confirm(options.operation().compare(apiliteral::operation::writedocument) == 0);

  if (options.operation().compare(apiliteral::operation::writedocument) != 0) {
    throw runtime_error("write_document") << "Wrong operation option; operation=<" << options.operation() << ">.";
  }

  if (options.provider().compare(apiliteral::provider::ucon) != 0) {
    throw runtime_error("write_document") << "Wrong provider option; provider=<" << options.provider() << ">.";
  }

  TRACE_(15, "writeDocument(): begin");

  auto config = DBI::DBConfig{};
  auto database = DBI::DB::create(config);
  auto provider = DBI::DBProvider<JsonData>::create(database);
  auto object_id = provider->writeDocument(insert_payload);

  TRACE_(15, "writeDocument(): object_id=<" << object_id << ">");

  TRACE_(15, "writeDocument(): end");
}

JsonData prov::readDocument(ManageDocumentOperation const& options, JsonData const& search_payload) {
  confirm(options.provider().compare(apiliteral::provider::ucon) == 0);
  confirm(options.operation().compare(apiliteral::operation::readdocument) == 0);

  if (options.operation().compare(apiliteral::operation::readdocument) != 0) {
    throw runtime_error("read_document") << "Wrong operation option; operation=<" << options.operation() << ">.";
  }

  if (options.provider().compare(apiliteral::provider::ucon) != 0) {
    throw runtime_error("read_document") << "Wrong provider option; provider=<" << options.provider() << ">.";
  }

  TRACE_(16, "readDocument(): begin");

  auto config = DBI::DBConfig{};
  auto database = DBI::DB::create(config);
  auto provider = DBI::DBProvider<JsonData>::create(database);

  auto collection = provider->readDocument(search_payload);

  TRACE_(16, "read_document: "
                 << "Search returned " << collection.size() << " results.");

  if (collection.size() != 1) {
    throw runtime_error("read_document") << "Search returned " << collection.size() << " results.";
  }

  auto data = JsonData{*collection.begin() };

  TRACE_(16, "readDocument(): end");

  return data;
}

JsonData prov::findConfigurations(ManageDocumentOperation const& options, JsonData const& search_payload) {
  confirm(options.provider().compare(apiliteral::provider::ucon) == 0);
  confirm(options.operation().compare(apiliteral::operation::findconfigs) == 0);

  if (options.operation().compare(apiliteral::operation::findconfigs) != 0) {
    throw runtime_error("operation_findconfigs") << "Wrong operation option; operation=<" << options.operation()
                                                 << ">.";
  }

  if (options.provider().compare(apiliteral::provider::ucon) != 0) {
    throw runtime_error("operation_findconfigs") << "Wrong provider option; provider=<" << options.provider() << ">.";
  }

  TRACE_(17, "operation_findconfigs: begin");

  auto config = DBI::DBConfig{};
  auto database = DBI::DB::create(config);
  auto provider = DBI::DBProvider<JsonData>::create(database);

  auto global_configs = provider->findConfigurations(search_payload);

  if (global_configs.empty()) {
    return {apiliteral::empty_search_result};
    // throw runtime_error("operation_findconfigs") << "No global
    // configurations were found.";
  }

  auto needComma = bool{false};
  auto printComma = [&needComma]() {
    if (needComma) return ", ";
    needComma = true;
    return " ";
  };

  std::ostringstream oss;

  auto seenValues = std::list<std::string>{};
  seenValues.push_back(apiliteral::notprovided);

  auto isNew = [& v = seenValues](auto const& name) {
    confirm(!name.empty());
    if (std::find(v.begin(), v.end(), name) != v.end()) return false;

    v.emplace_back(name);
    return true;
  };

  oss << "{ \"search\": [";

  for (auto const& global_config : global_configs) {
    auto doc = JSONDocument{global_config };

    auto global_config_name = JSONDocument::value(doc.findChild("filter").value());

    if (!isNew(global_config_name)) continue;

    oss << printComma() << "{";
    oss << "\"name\" :\"" << global_config_name << "\",";
    oss << "\"query\" :" << global_config ;
    oss << "}";
  }

  oss << "] }";

  return {oss.str()};
}

JsonData prov::configurationComposition(ManageDocumentOperation const& options, JsonData const& search_payload) {
  confirm(options.provider().compare(apiliteral::provider::ucon) == 0);
  confirm(options.operation().compare(apiliteral::operation::confcomposition) == 0);

  if (options.operation().compare(apiliteral::operation::confcomposition) != 0) {
    throw runtime_error("operation_confcomposition") << "Wrong operation option; operation=<" << options.operation()
                                                     << ">.";
  }

  if (options.provider().compare(apiliteral::provider::ucon) != 0) {
    throw runtime_error("operation_confcomposition") << "Wrong provider option; provider=<" << options.provider()
                                                     << ">.";
  }

  TRACE_(18, "operation_confcomposition: begin");

  auto config = DBI::DBConfig{};
  auto database = DBI::DB::create(config);
  auto provider = DBI::DBProvider<JsonData>::create(database);

  auto query_payloads = provider->configurationComposition(search_payload);

  if (query_payloads.empty()) {
    throw runtime_error("operation_confcomposition") << "No search filters were found.";
  }

  auto ex = std::regex(
      "\"configurations\\.name\"\\s*:\\s*\"((\\\\\"|[^\"])*)\"\\,"
      "\\s*\"entities\\.name\"\\s*:\\s*\"((\\\\\"|"
      "[^\"])*)\"");

  auto needComma = bool{false};
  auto printComma = [&needComma]() {
    if (needComma) return ", ";
    needComma = true;
    return " ";
  };

  std::ostringstream oss;

  oss << "{ \"search\": [\n";

  for (auto const& query_payload : query_payloads) {
    auto filter_json = JSONDocument{query_payload }.findChild("filter").value();

    auto results = std::smatch();

    if (!std::regex_search(filter_json, results, ex))
      throw runtime_error("operation_confcomposition") << "Unsupported filter string, no match";

    /*
        for (size_t i = 0; i < results.size(); ++i) {
          std::ssub_match sub_match = results[i];
          std::string piece = sub_match.str();
                     TRACE_(18, "operation_confcomposition: submatch*** " << i << ":
       " << piece << '\n');
        }
    */
    if (results.size() != 5)
      throw runtime_error("operation_confcomposition") << "Unsupported filter string, wrong result count";

    oss << printComma() << "{";
    oss << "\"name\" :\"" << results[1].str() << ":" << results[3].str() << "\",";
    oss << "\"query\" :" << query_payload ;
    oss << "}\n";
  }

  oss << "] }";

  return {oss.str()};
}

JsonData prov::findVersions(ManageDocumentOperation const& options, JsonData const&) {
  confirm(options.provider().compare(apiliteral::provider::ucon) == 0);
  confirm(options.operation().compare(apiliteral::operation::findversions) == 0);

  if (options.operation().compare(apiliteral::operation::findversions) != 0) {
    throw runtime_error("operation_findversions") << "Wrong operation option; operation=<" << options.operation()
                                                  << ">.";
  }

  if (options.provider().compare(apiliteral::provider::ucon) != 0) {
    throw runtime_error("operation_findversions") << "Wrong provider option; provider=<" << options.provider() << ">.";
  }

  TRACE_(19, "operation_findversions: begin");

  auto config = DBI::DBConfig{};
  auto database = DBI::DB::create(config);
  auto provider = DBI::DBProvider<JsonData>::create(database);

  auto query_payload = options.to_JsonData();
  auto config_versions = provider->findVersions(query_payload);

  if (config_versions.empty()) {
    return {apiliteral::empty_search_result};
    // throw runtime_error("operation_findversions") << "No configuration
    // versions were found.";
  }

  auto ex = std::regex(
      "\"version\"\\s*:\\s*\"((\\\\\"|[^\"])*)\"\\,\\s*"
      "\"entities\\.name\"\\s*:\\s*\"((\\\\\"|"
      "[^\"])*)\"");

  auto needComma = bool{false};
  auto printComma = [&needComma]() {
    if (needComma) return ", ";
    needComma = true;
    return " ";
  };

  std::ostringstream oss;

  oss << "{ \"search\": [";

  for (auto const& config_version : config_versions) {
    auto filter_json = JSONDocument{config_version }.findChild("filter").value();

    auto results = std::smatch();

    if (!std::regex_search(filter_json, results, ex))
      throw runtime_error("operation_findversions") << "Unsupported filter string, no match";

    /*
            for (size_t i = 0; i < results.size(); ++i) {
              std::ssub_match sub_match = results[i];
              std::string piece = sub_match.str();
                         TRACE_(18, "operation_findversions: submatch*** " << i
       << ": " << piece << '\n');
            }
    */
    if (results.size() != 5)
      throw runtime_error("operation_findversions") << "Unsupported filter string, wrong result count";

    oss << printComma() << "{";
    oss << "\"name\" :\"" << results[1].str() /*<< ":" << results[3].str()*/
        << "\",";
    oss << "\"query\" :" << config_version ;
    oss << "}";
  }

  oss << "] }";

  return {oss.str()};
}

JsonData prov::findEntities(ManageDocumentOperation const& options, JsonData const&) {
  confirm(options.provider().compare(apiliteral::provider::ucon) == 0);
  confirm(options.operation().compare(apiliteral::operation::findentities) == 0);

  if (options.operation().compare(apiliteral::operation::findentities) != 0) {
    throw runtime_error("operation_findentities") << "Wrong operation option; operation=<" << options.operation()
                                                  << ">.";
  }

  if (options.provider().compare(apiliteral::provider::ucon) != 0) {
    throw runtime_error("operation_findentities") << "Wrong provider option; provider=<" << options.provider() << ">.";
  }

  TRACE_(19, "operation_findentities: begin");

  auto config = DBI::DBConfig{};
  auto database = DBI::DB::create(config);
  auto provider = DBI::DBProvider<JsonData>::create(database);

  auto query_payload = options.to_JsonData();
  auto config_entities = provider->findEntities(query_payload);

  if (config_entities.empty()) {
    return {apiliteral::empty_search_result};
    // throw runtime_error("operation_findentities") << "No configuration
    // entities were found.";
  }

  auto ex = std::regex("\\s\"(entities\\.name)\"\\s:\\s\"((\\\\\"|[^\"])*)\"");

  auto needComma = bool{false};
  auto printComma = [&needComma]() {
    if (needComma) return ", ";
    needComma = true;
    return " ";
  };

  std::ostringstream oss;

  oss << "{ \"search\": [";

  for (auto const& config_entity : config_entities) {
    auto filter_json = JSONDocument{config_entity }.findChild("filter").value();
    TRACE_(18, "operation_findentities: filter_json=<" << filter_json << '>');

    auto results = std::smatch();

    if (!std::regex_search(filter_json, results, ex))
      throw runtime_error("operation_findentities") << "Unsupported filter string, no match";

    /*

            for (size_t i = 0; i < results.size(); ++i) {
              std::ssub_match sub_match = results[i];
              std::string piece = sub_match.str();
                         TRACE_(18, "operation_findentities: submatch*** " << i
       << ": " << piece << '\n');
            }
    */

    if (results.size() != 4)
      throw runtime_error("operation_findentities") << "Unsupported filter string, wrong result count";

    oss << printComma() << "{";
    oss << "\"name\" :\"" << results[2].str() /*<< ":" << results[3].str()*/
        << "\",";
    oss << "\"query\" :" << config_entity ;
    oss << "}";
  }

  oss << "] }";

  return {oss.str()};
}

JsonData prov::assignConfiguration(ManageDocumentOperation const& options, JsonData const& search_payload) {
  confirm(options.provider().compare(apiliteral::provider::ucon) == 0);
  confirm(options.operation().compare(apiliteral::operation::assignconfig) == 0);

  if (options.operation().compare(apiliteral::operation::assignconfig) != 0) {
    throw runtime_error("operation_addconfig") << "Wrong operation option; operation=<" << options.operation() << ">.";
  }

  if (options.provider().compare(apiliteral::provider::ucon) != 0) {
    throw runtime_error("operation_addconfig") << "Wrong provider option; provider=<" << options.provider() << ">.";
  }

  TRACE_(20, "operation_addconfig: begin");

  auto new_options = options;
  new_options.operation(apiliteral::operation::readdocument);

  auto search =
      JsonData{"{\"filter\":" + search_payload.json_buffer + ", \"collection\":\"" + options.collection() + "\"}"};
  TRACE_(20, "operation_addconfig: args search_payload=<" << search<< ">");

  auto document = ucon::readDocument(new_options, search);
  auto json_document = JSONDocument{document};
  JSONDocumentBuilder builder{json_document};
  auto configuration = JSONDocument{new_options.configuration_to_JsonData()};

  builder.addConfiguration(configuration);

  new_options.operation(apiliteral::operation::writedocument);

  auto update =
      JsonData{"{\"filter\":{\"$oid\":\"" + builder.extract().deleteChild("_id").value() + "\"},  \"document\":" +
               builder.extract().to_string() + ", \"collection\":\"" + options.collection() + "\"}"};

  ucon::writeDocument(new_options, update);

  new_options.operation(apiliteral::operation::confcomposition);

  auto find_options = ManageDocumentOperation{apiliteral::operation::assignconfig};

  find_options.operation(apiliteral::operation::confcomposition);
  find_options.format(options::data_format_t::gui);
  find_options.provider(apiliteral::provider::ucon);
  find_options.configuration(new_options.configuration());

  return ucon::configurationComposition(find_options, options.configuration_to_JsonData());
}

JsonData prov::removeConfiguration(ManageDocumentOperation const& options, JsonData const& search_payload) {
  confirm(options.provider().compare(apiliteral::provider::ucon) == 0);
  confirm(options.operation().compare(apiliteral::operation::removeconfig) == 0);

  if (options.operation().compare(apiliteral::operation::removeconfig) != 0) {
    throw runtime_error("operation_removeconfig") << "Wrong operation option; operation=<" << options.operation()
                                                  << ">.";
  }

  if (options.provider().compare(apiliteral::provider::ucon) != 0) {
    throw runtime_error("operation_removeconfig") << "Wrong provider option; provider=<" << options.provider() << ">.";
  }

  TRACE_(20, "operation_removeconfig: begin");

  auto new_options = options;
  new_options.operation(apiliteral::operation::readdocument);

  auto search =
      JsonData{"{\"filter\":" + search_payload.json_buffer + ", \"collection\":\"" + options.collection() + "\"}"};
  TRACE_(20, "operation_addconfig: args search_payload=<" << search<< ">");

  auto document = ucon::readDocument(new_options, search);
  auto json_document = JSONDocument{document};
  JSONDocumentBuilder builder{json_document};
  auto configuration = JSONDocument{new_options.configuration_to_JsonData()};

  builder.removeConfiguration(configuration);

  new_options.operation(apiliteral::operation::writedocument);

  auto update =
      JsonData{"{\"filter\":{\"$oid\":\"" + builder.extract().deleteChild("_id").value() + "\"},  \"document\":" +
               builder.extract().to_string() + ", \"collection\":\"" + options.collection() + "\"}"};

  ucon::writeDocument(new_options, update);

  new_options.operation(apiliteral::operation::confcomposition);

  auto find_options = ManageDocumentOperation{apiliteral::operation::removeconfig};

  find_options.operation(apiliteral::operation::confcomposition);
  find_options.format(options::data_format_t::gui);
  find_options.provider(apiliteral::provider::ucon);
  find_options.configuration(new_options.configuration());

  return ucon::configurationComposition(find_options, options.configuration_to_JsonData());
}
JsonData prov::listCollections(ManageDocumentOperation const& options, JsonData const& search_payload) {
  confirm(options.provider().compare(apiliteral::provider::ucon) == 0);
  confirm(options.operation().compare(apiliteral::operation::listcollections) == 0);

  if (options.operation().compare(apiliteral::operation::listcollections) != 0) {
    throw runtime_error("operation_listcollections") << "Wrong operation option; operation=<" << options.operation()
                                                     << ">.";
  }

  if (options.provider().compare(apiliteral::provider::ucon) != 0) {
    throw runtime_error("operation_listcollections") << "Wrong provider option; provider=<" << options.provider()
                                                     << ">.";
  }

  TRACE_(20, "operation_listcollections: begin");

  auto config = DBI::DBConfig{};
  auto database = DBI::DB::create(config);
  auto provider = DBI::DBProvider<JsonData>::create(database);

  auto collection_names = provider->listCollections(search_payload);

  if (collection_names.empty()) {
    return {apiliteral::empty_search_result};
    // throw runtime_error("operation_listcollections") << "No configuration
    // entities were found.";
  }

  auto needComma = bool{false};
  auto printComma = [&needComma]() {
    if (needComma) return ", ";
    needComma = true;
    return " ";
  };

  std::ostringstream oss;

  oss << "{ \"search\": [\n";

  for (auto const& collection_name : collection_names) {
    auto name = JSONDocument{collection_name }.findChild("collection").value();
    TRACE_(18, "operation_listcollections: collection=<" << name << '>');

    oss << printComma() << "{";
    oss << "\"name\" :\"" << name << "\",";
    oss << "\"query\" :" << collection_name ;
    oss << "}\n";
  }

  oss << "] }";

  return {oss.str()};
}

JsonData prov::listDatabases(ManageDocumentOperation const& options, JsonData const& search_payload) {
  confirm(options.provider().compare(apiliteral::provider::ucon) == 0);
  confirm(options.operation().compare(apiliteral::operation::listdatabases) == 0);

  if (options.operation().compare(apiliteral::operation::listdatabases) != 0) {
    throw runtime_error("operation_listdatabases") << "Wrong operation option; operation=<" << options.operation()
                                                   << ">.";
  }

  if (options.provider().compare(apiliteral::provider::ucon) != 0) {
    throw runtime_error("operation_listdatabases") << "Wrong provider option; provider=<" << options.provider() << ">.";
  }

  TRACE_(20, "operation_listdatabases: begin");

  auto config = DBI::DBConfig{};
  auto database = DBI::DB::create(config);
  auto provider = DBI::DBProvider<JsonData>::create(database);

  auto database_names = provider->listDatabases(search_payload);

  if (database_names.empty()) {
    return {apiliteral::empty_search_result};
    // throw runtime_error("operation_listdatabases") << "No configuration
    // entities were found.";
  }

  auto needComma = bool{false};
  auto printComma = [&needComma]() {
    if (needComma) return ", ";
    needComma = true;
    return " ";
  };

  std::ostringstream oss;

  oss << "{ \"search\": [\n";

  for (auto const& database_name : database_names) {
    auto name = JSONDocument{database_name }.findChild("database").value();
    TRACE_(18, "operation_listdatabases: database=<" << name << '>');

    oss << printComma() << "{";
    oss << "\"name\" :\"" << name << "\",";
    oss << "\"query\" :" << database_name ;
    oss << "}\n";
  }

  oss << "] }";

  return {oss.str()};
}

JsonData prov::readDbInfo(ManageDocumentOperation const& options, JsonData const& search_payload) {
  confirm(options.provider().compare(apiliteral::provider::ucon) == 0);
  confirm(options.operation().compare(apiliteral::operation::readdbinfo) == 0);

  if (options.operation().compare(apiliteral::operation::readdbinfo) != 0) {
    throw runtime_error("operation_readdbinfo") << "Wrong operation option; operation=<" << options.operation() << ">.";
  }

  if (options.provider().compare(apiliteral::provider::ucon) != 0) {
    throw runtime_error("operation_readdbinfo") << "Wrong provider option; provider=<" << options.provider() << ">.";
  }

  TRACE_(20, "operation_readdbinfo: begin");

  auto config = DBI::DBConfig{};
  auto database = DBI::DB::create(config);
  auto provider = DBI::DBProvider<JsonData>::create(database);

  auto search_results = provider->databaseMetadata(search_payload);

  if (search_results.empty()) {
    return {apiliteral::empty_search_result};
    // throw runtime_error("operation_listdatabases") << "No database info found.";
  }

  auto const database_metadata = search_results.begin();

  TRACE_(20, "operation_readdbinfo: database_metadata =<" << *database_metadata  << ">");

  std::ostringstream oss;
  oss << "{ \"search\":\n";
  oss << *database_metadata ;
  oss << "\n}";

  return {oss.str()};
}

JsonData prov::findVersionAliases(cf::ManageAliasesOperation const& /*options*/, JsonData const& /*query_payload*/) {
  throw runtime_error("findVersionAliases") << "findVersionAliases: is not implemented";
  return {apiliteral::empty_json};
}

void cf::debug::UconDB() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);

  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TRACE_(0, "artdaq::database::configuration::UconDB trace_enable");
}
