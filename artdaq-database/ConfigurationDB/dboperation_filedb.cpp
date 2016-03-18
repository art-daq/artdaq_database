#include "artdaq-database/ConfigurationDB/common.h"

#include "artdaq-database/ConfigurationDB/dboperation_filedb.h"
#include "artdaq-database/ConfigurationDB/dboperation_findconfigs.h"
#include "artdaq-database/ConfigurationDB/dboperation_loadstore.h"

#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/FhiclJson/shared_literals.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "COFS:OpLdStr_C"

namespace DBI = artdaq::database::filesystem;

namespace db = artdaq::database;
namespace cf = db::configuration;
namespace cfo = cf::options;
namespace cfd = cf::filesystem;

using artdaq::database::basictypes::JsonData;
using artdaq::database::jsonutils::JSONDocumentBuilder;
using artdaq::database::jsonutils::JSONDocument;

void cfd::store(cfo::LoadStoreOperation const& options, JsonData const& insert_payload) {
  assert(options.provider().compare(cfo::literal::database_provider_filesystem) == 0);
  assert(options.operation().compare(cfo::literal::operation_store) == 0);

  if (options.operation().compare(cfo::literal::operation_store) != 0) {
    throw cet::exception("store_configuration") << "Wrong operation option; operation=<" << options.operation() << ">.";
  }

  if (options.provider().compare(cfo::literal::database_provider_filesystem) != 0) {
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

JsonData cfd::load(cfo::LoadStoreOperation const& options, JsonData const& search_payload) {
  assert(options.provider().compare(cfo::literal::database_provider_filesystem) == 0);
  assert(options.operation().compare(cfo::literal::operation_load) == 0);

  if (options.operation().compare(cfo::literal::operation_load) != 0) {
    throw cet::exception("load_configuration") << "Wrong operation option; operation=<" << options.operation() << ">.";
  }

  if (options.provider().compare(cfo::literal::database_provider_filesystem) != 0) {
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

  auto data = JsonData(collection.begin()->json_buffer);

  TRACE_(16, "load: end");

  return data;
}

JsonData cfd::findGlobalConfigs(cfo::FindConfigsOperation const& options, JsonData const& search_payload) {
  assert(options.provider().compare(cfo::literal::database_provider_filesystem) == 0);
  assert(options.operation().compare(cfo::literal::operation_findconfigs) == 0);

  if (options.operation().compare(cfo::literal::operation_findconfigs) != 0) {
    throw cet::exception("operation_findconfigs") << "Wrong operation option; operation=<" << options.operation()
                                                  << ">.";
  }

  if (options.provider().compare(cfo::literal::database_provider_filesystem) != 0) {
    throw cet::exception("operation_findconfigs") << "Wrong provider option; provider=<" << options.provider() << ">.";
  }

  TRACE_(17, "operation_findconfigs: begin");
  TRACE_(18, "operation_findconfigs args data=<" << search_payload.json_buffer << ">");

  auto config = DBI::DBConfig{};
  auto database = DBI::DB::create(config);
  auto provider = DBI::DBProvider<JsonData>::create(database);

  auto global_configs = provider->findGlobalConfigs(search_payload);

  if (global_configs.empty()) {
    throw cet::exception("operation_findconfigs") << "No global configurations were found.";
  }

  auto printComma = bool{false};

  std::stringstream ss;

  auto seenValues = std::list<std::string>{};

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

    ss << (printComma ? ", " : " ") << "{";
    ss << "\"name\" :\"" << global_config_name << "\",";
    ss << "\"query\" :" << global_config.json_buffer;
    ss << "}";

    printComma = true;
  }

  ss << "] }";

  return {ss.str()};
}

JsonData cfd::buildConfigSearchFilter(cfo::FindConfigsOperation const& options, JsonData const& search_payload) {
  assert(options.provider().compare(cfo::literal::database_provider_filesystem) == 0);
  assert(options.operation().compare(cfo::literal::operation_buildfilter) == 0);

  if (options.operation().compare(cfo::literal::operation_buildfilter) != 0) {
    throw cet::exception("operation_buildfilter") << "Wrong operation option; operation=<" << options.operation()
                                                  << ">.";
  }

  if (options.provider().compare(cfo::literal::database_provider_filesystem) != 0) {
    throw cet::exception("operation_buildfilter") << "Wrong provider option; provider=<" << options.provider() << ">.";
  }

  TRACE_(18, "operation_buildfilter: begin");
  TRACE_(18, "operation_buildfilter args data=<" << search_payload.json_buffer << ">");

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

  auto printComma = bool{false};

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
                     TRACE_(18, "operation_buildfilter: submatch*** " << i << ": " << piece << '\n';
        }
    */
    if (results.size() != 5)
      throw cet::exception("operation_buildfilter") << "Unsupported filter string, wrong result count";

    ss << (printComma ? ", " : " ") << "{";
    ss << "\"name\" :\"" << results[1].str() << ":" << results[3].str() << "\",";
    ss << "\"query\" :" << search_filter.json_buffer;
    ss << "}";

    printComma = true;
  }

  ss << "] }";

  return {ss.str()};
}

void cf::trace_enable_DBOperationFileSystem() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", 1LL);
  TRACE_CNTL("modeS", 1LL);

  TRACE_(0, "artdaq::database::configuration::FileSystem"
                << "trace_enable");
}
