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

JsonData cfd::findGlobalConfigs(cfo::FindConfigsOperation const& options[[gnu::unused]],
                                             JsonData const& search_payload[[gnu::unused]]) {
  assert(options.provider().compare(cfo::literal::database_provider_mongo) == 0);
  assert(options.operation().compare(cfo::literal::operation_findconfigs) == 0);

  auto global_configs = JsonData{"{}"};

  return global_configs;
}

JsonData cfd::buildConfigSearchFilter(cfo::FindConfigsOperation const& options[[gnu::unused]],
                                                   JsonData const& search_payload[[gnu::unused]]) {
  assert(options.provider().compare(cfo::literal::database_provider_mongo) == 0);
  assert(options.operation().compare(cfo::literal::database_provider_mongo) == 0);

  auto global_configs_filters = JsonData{"{}"};

  return global_configs_filters;
}

void cf::trace_enable_DBOperationFileSystem() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", 1LL);
  TRACE_CNTL("modeS", 1LL);

  TRACE_(0, "artdaq::database::configuration::FileSystem"
                << "trace_enable");
}
