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
    : uri{std::string{literal::MONGOURI} + literal::hostname + ":" + std::to_string(literal::port) + "/" + literal::db_name} {
  auto tmpURI =
      getenv("ARTDAQ_DATABASE_URI") ? expand_environment_variables("${ARTDAQ_DATABASE_URI}") : std::string("");

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