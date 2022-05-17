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

#include <boost/filesystem.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/options/find.hpp>
#include <mongocxx/pipeline.hpp>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <utility>

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "provider_connection.cpp"

using namespace artdaq::database;

using artdaq::database::mongo::DBConfig;
using artdaq::database::mongo::MongoDB;

mongocxx::instance& getInstance() {
  static mongocxx::instance _instance{};
  return _instance;
}

DBConfig::DBConfig() : uri{std::string{literal::MONGOURI} + literal::hostname + ":" + std::to_string(literal::port) + "/" + literal::db_name} {
  auto tmpURI = getenv("ARTDAQ_DATABASE_URI") != nullptr ? expand_environment_variables("${ARTDAQ_DATABASE_URI}") : std::string("");

  auto prefixURI = std::string{literal::MONGOURI};
  if (tmpURI.length() > prefixURI.length() && std::equal(prefixURI.begin(), prefixURI.end(), tmpURI.begin())) {
    uri = tmpURI;
  }

  auto clientCert =
      getenv("ARTDAQ_DATABASE_CLIENT_CERT") != nullptr ? expand_environment_variables("${ARTDAQ_DATABASE_CLIENT_CERT}") : std::string("");
  if (!clientCert.empty() && boost::filesystem::exists(boost::filesystem::path(clientCert))) {
    uri += "&tls=true&authMechanism=MONGODB-X509&tlsCertificateKeyFile=" + clientCert;

    auto caCert = getenv("ARTDAQ_DATABASE_CA_CERT") != nullptr ? expand_environment_variables("${ARTDAQ_DATABASE_CA_CERT}") : std::string("");
    if (!caCert.empty() && boost::filesystem::exists(boost::filesystem::path(caCert))) uri += "&tlsCAFile=" + caCert;
  }
}

std::shared_ptr<MongoDB> MongoDB::create(DBConfig const& config) {
  // reuse mongodb connection
  static std::mutex instances_mutex;
  static auto instances = std::unordered_map<std::string, std::shared_ptr<MongoDB>>();

  std::ostringstream oss;
  oss << "Thread=" << std::this_thread::get_id() << ",URI=" << config.connectionURI();
  auto key = oss.str();

  TLOG(11) << "StorageProvider::MongoDB::create " << key;

  std::lock_guard<std::mutex> guard(instances_mutex);
  return instances.try_emplace(key, std::make_shared<MongoDB, DBConfig const&, PassKeyIdiom const&>(config, {})).first->second;
}

DBConfig::DBConfig(const std::string& uri_) : uri{uri_} { confirm(!uri_.empty()); }

MongoDB::MongoDB(DBConfig config, PassKeyIdiom const& /*unused*/)
    : _config{std::move(config)},
      _instance{getInstance()},
      _client{mongocxx::uri{_config.connectionURI()}},
      _connection{_client[_client.uri().database()]} {
  _client.start_session();
}

mongocxx::cursor MongoDB::list_databases() {
  connection();
  return _client.list_databases();
}

mongocxx::database& MongoDB::connection() {
  // if (_connection.has_collection(system_metadata)) return _connection;

  auto collection = _connection.collection(system_metadata);
  auto filter = bsoncxx::builder::core(false);

  if (collection.count_documents(filter.view_document()) > 0) {
    return _connection;
  }

  auto bson_document = compat::from_json("{" + make_database_metadata("artdaq", expand_environment_variables(_config.connectionURI())) + "}");

  auto result = collection.insert_one(bson_document.view());

  auto object_id = object_id_t(compat::to_json(result->inserted_id()));

  TLOG(12) << "StorageProvider::MongoDB::connection created metadata record id=" << object_id;

  return _connection;
}

namespace artdaq {
namespace database {
namespace mongo {
namespace debug {
void connection() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TLOG(10) << "artdaq::database::mongo trace_enable";
}
}  // namespace debug
}  // namespace mongo
}  // namespace database
}  // namespace artdaq
