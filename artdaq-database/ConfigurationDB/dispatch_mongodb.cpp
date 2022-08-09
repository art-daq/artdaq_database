#include "artdaq-database/ConfigurationDB/common.h"

#include "artdaq-database/ConfigurationDB/dispatch_mongodb.h"
#include "artdaq-database/ConfigurationDB/options_operations.h"

#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/DataFormats/shared_literals.h"
#include "artdaq-database/JsonDocument/JSONDocument.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/SharedCommon/helper_functions.h"
#include "artdaq-database/StorageProviders/MongoDB/provider_mongodb.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "dispatch_mongodb.cpp"

using namespace artdaq::database::configuration;
namespace cf = artdaq::database::configuration;
namespace db = artdaq::database;

namespace DBI = artdaq::database::mongo;
namespace prov = artdaq::database::configuration::mongo;
namespace docrec = artdaq::database::docrecord;

using artdaq::database::docrecord::JSONDocument;
using artdaq::database::docrecord::JSONDocumentBuilder;

void prov::writeDocument(ManageDocumentOperation const& options, JSONDocument const& insert_payload) {
  if (options.operation() != apiliteral::operation::writedocument && options.operation() != apiliteral::operation::overwritedocument) {
    throw runtime_error("write_document") << "Wrong operation option; operation=<" << options.operation() << ">.";
  }

  if (options.provider() != apiliteral::provider::mongo) {
    throw runtime_error("write_document") << "Wrong provider option; provider=<" << options.provider() << ">.";
  }

  TLOG(25) << "writeDocument(): begin";

  auto config = DBI::DBConfig{};
  auto database = DBI::DB::create(config);
  auto provider = DBI::DBProvider<JSONDocument>::create(database);
  auto object_id = provider->writeDocument(insert_payload);

  TLOG(25) << "writeDocument(): object_id=<" << object_id << ">";

  TLOG(25) << "writeDocument(): end";
}

JSONDocument prov::readDocument(ManageDocumentOperation const& options, JSONDocument const& search_payload) {
  TLOG(26) << "readDocument(): begin";

  auto collection = readDocuments(options, search_payload);

  TLOG(26) << "read_document: "
           << "Search returned " << collection.size() << " results.";

  if (collection.empty()) {
    throw runtime_error("read_document") << "No documents found.";
  }

  auto data = JSONDocument{};

  std::swap(data,
            collection.size() > 1 ? *std::max_element(collection.begin(), collection.end(), docrec::compareDocumentVersions) : *collection.begin());

  TLOG(26) << "readDocument(): end";

  return data;
}

std::vector<JSONDocument> prov::readDocuments(ManageDocumentOperation const& options, JSONDocument const& search_payload) {
  if (options.operation() != apiliteral::operation::readdocument) {
    throw runtime_error("read_documents") << "Wrong operation option; operation=<" << options.operation() << ">.";
  }

  if (options.provider() != apiliteral::provider::mongo) {
    throw runtime_error("read_documents") << "Wrong provider option; provider=<" << options.provider() << ">.";
  }

  TLOG(26) << "readDocuments(): begin";

  auto config = DBI::DBConfig{};
  auto database = DBI::DB::create(config);
  auto provider = DBI::DBProvider<JSONDocument>::create(database);

  auto collection = provider->readDocument(search_payload);

  TLOG(26) << "read_documents: Search returned " << collection.size() << " results.";

  TLOG(26) << "readDocuments(): end";

  return collection;
}
std::vector<JSONDocument> prov::findConfigurations(ManageDocumentOperation const& options, JSONDocument const& search_payload) {
  auto returnValue = std::vector<JSONDocument>{};

  if (options.operation() != apiliteral::operation::findconfigs) {
    throw runtime_error("operation_findconfigs") << "Wrong operation option; operation=<" << options.operation() << ">.";
  }

  if (options.provider() != apiliteral::provider::mongo) {
    throw runtime_error("operation_findconfigs") << "Wrong provider option; provider=<" << options.provider() << ">.";
  }

  TLOG(27) << "operation_findconfigs: begin";

  auto config = DBI::DBConfig{};
  auto database = DBI::DB::create(config);
  auto provider = DBI::DBProvider<JSONDocument>::create(database);

  auto search_results = provider->findConfigurations(search_payload);

  auto seenValues = std::list<std::string>{};
  seenValues.emplace_back(apiliteral::notprovided);

  auto isNew = [&v = seenValues](auto const& name) {
    confirm(!name.empty());
    if (std::find(v.begin(), v.end(), name) != v.end()) {
      return false;
    }

    v.emplace_back(name);
    return true;
  };

  for (auto const& search_result : search_results) {
    auto name = JSONDocument::value(search_result.findChild("filter").value());

    if (!isNew(name)) {
      continue;
    }

    TLOG(28) << "operation_findconfigs: configuration=<" << name << ">";

    std::ostringstream oss;
    oss << "{";
    oss << quoted_(jsonliteral::name) << ":" << quoted_(name) << ",";
    oss << quoted_(jsonliteral::query) << ":" << search_result;
    oss << "}\n";

    returnValue.emplace_back(oss.str());
  }

  TLOG(30) << "operation_listcollections: end";

  return returnValue;
}

JSONDocument prov::configurationComposition(ManageDocumentOperation const& options, JSONDocument const& search_payload) {
  if (options.operation() != apiliteral::operation::confcomposition) {
    throw runtime_error("operation_confcomposition") << "Wrong operation option; operation=<" << options.operation() << ">.";
  }

  if (options.provider() != apiliteral::provider::mongo) {
    throw runtime_error("operation_confcomposition") << "Wrong provider option; provider=<" << options.provider() << ">.";
  }

  TLOG(28) << "operation_confcomposition: begin";

  auto config = DBI::DBConfig{};
  auto database = DBI::DB::create(config);
  auto provider = DBI::DBProvider<JSONDocument>::create(database);

  auto query_payloads = provider->configurationComposition(search_payload);

  TLOG(28) << "operation_confcomposition: configurationComposition returned " << query_payloads.size() << " results.";

  if (query_payloads.empty()) {
    throw runtime_error("operation_confcomposition") << "No results found for " << options.configuration();
  }

  auto ex = std::regex(
      "\"configurations\\.name\"\\s*:\\s*\"((\\\\\"|[^\"])*)\"\\,"
      "\\s*\"entities\\.name\"\\s*:\\s*\"((\\\\\"|"
      "[^\"])*)\"");

  auto needComma = bool{false};
  auto printComma = [&needComma]() {
    if (needComma) {
      return ", ";
    }
    needComma = true;
    return " ";
  };

  std::ostringstream oss;

  oss << "{ \"search\": [\n";

  for (auto const& query_payload : query_payloads) {
    auto filter_json = JSONDocument{query_payload}.findChild("filter").value();

    auto results = std::smatch();

    if (!std::regex_search(filter_json, results, ex)) {
      throw runtime_error("operation_confcomposition") << "Unsupported filter string, no match";
    }

#ifdef EXTRA_TRACES
    for (size_t i = 0; i < results.size(); ++i) {
      std::ssub_match sub_match = results[i];
      std::string piece = sub_match.str();
      TLOG(28) << "operation_confcomposition: submatch*** " << i << ":" << piece;
    }
#endif

    if (results.size() != 5) {
      throw runtime_error("operation_confcomposition") << "Unsupported filter string, wrong result count";
    }

    oss << printComma() << "{";
    oss << quoted_("name") << ":\"" << results[1].str() << ":" << results[3].str() << "\",";
    oss << "\"query\" :" << query_payload;
    oss << "}\n";
  }

  oss << "] }";

  return {db::replace_all(oss.str(), "\\\\/", "/")};
}

std::vector<JSONDocument> prov::findVersions(ManageDocumentOperation const& options, JSONDocument const& /*unused*/) {
  auto returnValue = std::vector<JSONDocument>{};

  if (options.operation() != apiliteral::operation::findversions) {
    throw runtime_error("operation_findversions") << "Wrong operation option; operation=<" << options.operation() << ">.";
  }

  if (options.provider() != apiliteral::provider::mongo) {
    throw runtime_error("operation_findversions") << "Wrong provider option; provider=<" << options.provider() << ">.";
  }

  TLOG(30) << "operation_findversions: begin";

  auto config = DBI::DBConfig{};
  auto database = DBI::DB::create(config);
  auto provider = DBI::DBProvider<JSONDocument>::create(database);

  auto search_results = provider->findVersions<JSONDocument>(options);

  auto ex = std::regex(
      "\"version\"\\s*:\\s*\"((\\\\\"|[^\"])*)\"\\,\\s*"
      "\"entities\\.name\"\\s*:\\s*\"((\\\\\"|"
      "[^\"])*)\"");

  for (auto const& search_result : search_results) {
    auto filter_json = search_result.findChild("filter").value();

    TLOG(28) << "operation_findversions: filter_json=<" << filter_json << ">";

    auto results = std::smatch();

    if (!std::regex_search(filter_json, results, ex)) {
      throw runtime_error("operation_findversions") << "Unsupported filter string, no match";
    }

#ifdef EXTRA_TRACES
    for (size_t i = 0; i < results.size(); ++i) {
      std::ssub_match sub_match = results[i];
      std::string piece = sub_match.str();
      TLOG(28) << "operation_findversions: submatch*** " << i << ": " << piece;
    }
#endif

    if (results.size() != 5) {
      throw runtime_error("operation_findversions") << "Unsupported filter string, wrong result count";
    }

    TLOG(28) << "operation_findversions: version=<" << results[1].str() << ">";

    std::ostringstream oss;
    oss << "{";
    oss << quoted_(jsonliteral::name) << ":" << quoted_(results[1].str()) << ",";
    oss << quoted_(jsonliteral::query) << ":" << search_result;
    oss << "}\n";

    returnValue.emplace_back(db::replace_all(oss.str(), "\\\\/", "/"));
  }

  TLOG(30) << "operation_findversions: end";

  return returnValue;
}

std::vector<JSONDocument> prov::findEntities(ManageDocumentOperation const& options, JSONDocument const& /*unused*/) {
  auto returnValue = std::vector<JSONDocument>{};

  if (options.operation() != apiliteral::operation::findentities) {
    throw runtime_error("operation_findentities") << "Wrong operation option; operation=<" << options.operation() << ">.";
  }

  if (options.provider() != apiliteral::provider::mongo) {
    throw runtime_error("operation_findentities") << "Wrong provider option; provider=<" << options.provider() << ">.";
  }

  TLOG(29) << "operation_findentities: begin";

  auto config = DBI::DBConfig{};
  auto database = DBI::DB::create(config);
  auto provider = DBI::DBProvider<JSONDocument>::create(database);

  auto search_results = provider->findEntities<JSONDocument>(options);

  auto ex = std::regex(R"lit(\s"(entities\.name)"\s:\s"((\\"|[^"])*)")lit");

  for (auto const& search_result : search_results) {
    auto filter_json = search_result.findChild("filter").value();

    auto results = std::smatch();

    TLOG(28) << "operation_findentities: filter_json=<" << filter_json << ">";

    if (!std::regex_search(filter_json, results, ex)) {
      throw runtime_error("operation_findentities") << "Unsupported filter string, no match";
    }

#ifdef EXTRA_TRACES
    for (size_t i = 0; i < results.size(); ++i) {
      std::ssub_match sub_match = results[i];
      std::string piece = sub_match.str();
      TLOG(28) << "operation_findentities: submatch*** " << i << ": " << piece;
    }
#endif

    if (results.size() != 4) {
      throw runtime_error("operation_findentities") << "Unsupported filter string, wrong result count";
    }

    TLOG(28) << "operation_findentities: entity=<" << results[2].str() << ">";

    std::ostringstream oss;
    oss << "{";
    oss << quoted_(jsonliteral::name) << ":" << quoted_(results[2].str()) << ",";
    oss << quoted_(jsonliteral::query) << ":" << search_result;
    oss << "}\n";

    returnValue.emplace_back(oss.str());
  }

  TLOG(30) << "operation_findentities: end";

  return returnValue;
}

JSONDocument prov::assignConfiguration(ManageDocumentOperation const& options, JSONDocument const& search_payload) {
  if (options.operation() != apiliteral::operation::assignconfig) {
    throw runtime_error("operation_addconfig") << "Wrong operation option; operation=<" << options.operation() << ">.";
  }

  if (options.provider() != apiliteral::provider::mongo) {
    throw runtime_error("operation_addconfig") << "Wrong provider option; provider=<" << options.provider() << ">.";
  }

  TLOG(30) << "operation_addconfig: begin";

  auto new_options = options;
  new_options.operation(apiliteral::operation::readdocument);

  auto search_str = std::string{"{\"filter\":"} + search_payload.to_string() + ", \"collection\":\"" + options.collection() + "\"}";

  auto search = JSONDocument(search_str);
  TLOG(30) << "operation_addconfig: args search_payload=<" << search << ">";

  auto document = mongo::readDocument(new_options, search);
  JSONDocumentBuilder builder(document);
  auto configuration = JSONDocument{new_options.configuration_to_JsonData()};

  builder.addConfiguration(configuration);

  new_options.operation(apiliteral::operation::writedocument);
  auto update_str = std::string{"{\"filter\": "} + builder.getObjectID().to_string() + ",  \"document\":" + builder.to_string() + "}";
  auto update = JSONDocument(update_str);

  mongo::writeDocument(new_options, update);

  new_options.operation(apiliteral::operation::confcomposition);

  auto find_options = ManageDocumentOperation{apiliteral::operation::assignconfig};

  find_options.operation(apiliteral::operation::confcomposition);
  find_options.format(options::data_format_t::gui);
  find_options.provider(apiliteral::provider::mongo);
  find_options.configuration(new_options.configuration());

  return mongo::configurationComposition(find_options, options.configurationsname_to_JsonData());
}

JSONDocument prov::removeConfiguration(ManageDocumentOperation const& options, JSONDocument const& search_payload) {
  if (options.operation() != apiliteral::operation::removeconfig) {
    throw runtime_error("operation_removeconfig") << "Wrong operation option; operation=<" << options.operation() << ">.";
  }

  if (options.provider() != apiliteral::provider::mongo) {
    throw runtime_error("operation_removeconfig") << "Wrong provider option; provider=<" << options.provider() << ">.";
  }

  TLOG(30) << "operation_removeconfig: begin";

  auto new_options = options;
  new_options.operation(apiliteral::operation::readdocument);

  auto search_str = std::string{"{\"filter\":"} + search_payload.to_string() + ", \"collection\":\"" + options.collection() + "\"}";
  auto search = JSONDocument(search_str);
  TLOG(30) << "operation_addconfig: args search_payload=<" << search << ">";

  auto document = mongo::readDocument(new_options, search);
  JSONDocumentBuilder builder(document);
  auto configuration = JSONDocument{new_options.configuration_to_JsonData()};

  builder.removeConfiguration(configuration);

  new_options.operation(apiliteral::operation::writedocument);

  auto update_str = std::string{"({\"filter\": )"} + builder.getObjectID().to_string() + ",  \"document\":" + builder.to_string() + "}";
  auto update = JSONDocument(update_str);

  mongo::writeDocument(new_options, update);

  new_options.operation(apiliteral::operation::confcomposition);

  auto find_options = ManageDocumentOperation{apiliteral::operation::removeconfig};

  find_options.operation(apiliteral::operation::confcomposition);
  find_options.format(options::data_format_t::gui);
  find_options.provider(apiliteral::provider::mongo);
  find_options.configuration(new_options.configuration());

  return mongo::configurationComposition(find_options, options.configurationsname_to_JsonData());
}

std::vector<JSONDocument> prov::listCollections(ManageDocumentOperation const& options, JSONDocument const& search_payload) {
  auto returnValue = std::vector<JSONDocument>{};

  if (options.operation() != apiliteral::operation::listcollections) {
    throw runtime_error("operation_listcollections") << "Wrong operation option; operation=<" << options.operation() << ">.";
  }

  if (options.provider() != apiliteral::provider::mongo) {
    throw runtime_error("operation_listcollections") << "Wrong provider option; provider=<" << options.provider() << ">.";
  }

  TLOG(30) << "operation_listcollections: begin";

  auto config = DBI::DBConfig{};
  auto database = DBI::DB::create(config);
  auto provider = DBI::DBProvider<JSONDocument>::create(database);

  auto search_results = provider->listCollections(search_payload);

  for (auto const& search_result : search_results) {
    auto name = search_result.value_as<std::string>("collection");
    TLOG(28) << "operation_listcollections: collection=<" << name << ">";

    std::ostringstream oss;
    oss << "{";
    oss << quoted_(jsonliteral::name) << ":" << quoted_(name) << ",";
    oss << quoted_(jsonliteral::query) << ":" << search_result;
    oss << "}\n";

    returnValue.emplace_back(oss.str());
  }

  TLOG(30) << "operation_listcollections: end";

  return returnValue;
}

std::vector<JSONDocument> prov::listDatabases(ManageDocumentOperation const& options, JSONDocument const& search_payload) {
  auto returnValue = std::vector<JSONDocument>{};

  if (options.operation() != apiliteral::operation::listdatabases) {
    throw runtime_error("operation_listdatabases") << "Wrong operation option; operation=<" << options.operation() << ">.";
  }

  if (options.provider() != apiliteral::provider::mongo) {
    throw runtime_error("operation_listdatabases") << "Wrong provider option; provider=<" << options.provider() << ">.";
  }

  TLOG(30) << "operation_listdatabases: begin";

  auto config = DBI::DBConfig{};
  auto database = DBI::DB::create(config);
  auto provider = DBI::DBProvider<JSONDocument>::create(database);

  auto search_results = provider->listDatabases(search_payload);

  for (auto const& search_result : search_results) {
    auto name = search_result.value_as<std::string>("database");
    TLOG(28) << "operation_listdatabases: database=<" << name << ">";

    std::ostringstream oss;
    oss << "{";
    oss << quoted_(jsonliteral::name) << ":" << quoted_(name) << ",";
    oss << quoted_(jsonliteral::query) << ":" << search_result;
    oss << "}\n";

    returnValue.emplace_back(oss.str());
  }

  TLOG(30) << "operation_listdatabases: end";

  return returnValue;
}

JSONDocument prov::readDbInfo(ManageDocumentOperation const& options, JSONDocument const& search_payload) {
  if (options.operation() != apiliteral::operation::readdbinfo) {
    throw runtime_error("operation_readdbinfo") << "Wrong operation option; operation=<" << options.operation() << ">.";
  }

  if (options.provider() != apiliteral::provider::mongo) {
    throw runtime_error("operation_readdbinfo") << "Wrong provider option; provider=<" << options.provider() << ">.";
  }

  TLOG(30) << "operation_readdbinfo: begin";

  auto config = DBI::DBConfig{};
  auto database = DBI::DB::create(config);
  auto provider = DBI::DBProvider<JSONDocument>::create(database);

  auto search_results = provider->databaseMetadata(search_payload);

  if (search_results.empty()) {
    return JSONDocument{std::string{apiliteral::empty_json}};
  }

  auto const database_metadata = search_results.begin();

  TLOG(30) << "operation_readdbinfo: database_metadata =<" << *database_metadata << ">";

  TLOG(30) << "operation_readdbinfo: end";

  return *database_metadata;
}

std::vector<JSONDocument> prov::searchCollection(ManageDocumentOperation const& options, JSONDocument const& query_payload) {
  auto returnValue = std::vector<JSONDocument>{};

  if (options.operation() != apiliteral::operation::searchcollection) {
    throw runtime_error("operation_searchcollection") << "Wrong operation option; operation=<" << options.operation() << ">.";
  }

  if (options.provider() != apiliteral::provider::mongo) {
    throw runtime_error("operation_searchgollection") << "Wrong provider option; provider=<" << options.provider() << ">.";
  }

  TLOG(30) << "operation_searchcollection: begin";

  auto config = DBI::DBConfig{};
  auto database = DBI::DB::create(config);
  auto provider = DBI::DBProvider<JSONDocument>::create(database);

  auto search_results = provider->searchCollection(query_payload);

  for (auto const& search_result : search_results) {
    std::ostringstream oss;
    oss << "{";
    oss << quoted_(jsonliteral::query) << ":" << search_result;
    oss << "}\n";

    returnValue.emplace_back(oss.str());
  }

  TLOG(30) << "operation_listdatabases: end";

  return returnValue;
}

std::vector<JSONDocument> prov::findVersionAliases(cf::ManageAliasesOperation const& /*options*/, JSONDocument const& /*query_payload*/) {
  auto returnValue = std::vector<JSONDocument>{};

  throw runtime_error("findVersionAliases") << "findVersionAliases: is not implemented";

  return returnValue;
}

void cf::debug::MongoDB() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);

  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TLOG(10) << "artdaq::database::configuration::MongoDB trace_enable";
}
