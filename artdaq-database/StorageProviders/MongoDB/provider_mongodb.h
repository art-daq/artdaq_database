#ifndef _ARTDAQ_DATABASE_PROVIDER_MONGO_H_
#define _ARTDAQ_DATABASE_PROVIDER_MONGO_H_

#include "artdaq-database/StorageProviders/common.h"
#include "artdaq-database/StorageProviders/storage_providers.h"

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

namespace artdaq {
namespace database {
namespace mongo {
namespace debug {
void enable();
}

namespace literal {
constexpr auto MONGOURI = "mongodb://";
constexpr auto hostname = "127.0.0.1";
constexpr auto port = "27017";
constexpr auto db_name = "test_configuration_db";
}

struct DBConfig final {
  DBConfig() : uri{std::string{literal::MONGOURI} + literal::hostname + ":" + literal::port + "/" + literal::db_name} {
    auto tmpURI =
        getenv("ARTDAQ_DATABASE_URI") ? expand_environment_variables("${ARTDAQ_DATABASE_URI}") : std::string("");

    if (tmpURI.back() == '/') tmpURI.pop_back();  // remove trailing slash

    auto prefixURI = std::string{literal::MONGOURI};
    if (tmpURI.length() > prefixURI.length() && std::equal(prefixURI.begin(), prefixURI.end(), tmpURI.begin()))
      uri = tmpURI;
  }
  DBConfig(std::string uri_) : uri{uri_} { assert(!uri_.empty()); }
  std::string uri;
  const std::string connectionURI() const { return uri; };
};

class MongoDB final {
 public:
  mongocxx::database& connection();

  static std::shared_ptr<MongoDB> create(DBConfig const& config) {
    return std::make_shared<MongoDB, DBConfig const&, PassKeyIdiom const&>(config, {});
  }

  class PassKeyIdiom final {
   private:
    friend std::shared_ptr<MongoDB> MongoDB::create(DBConfig const&);
    PassKeyIdiom() = default;
  };

  explicit MongoDB(DBConfig const& config, PassKeyIdiom const&)
      : _config{config},
        _instance{},
        _client{mongocxx::uri{_config.connectionURI()}},
        _connection{_client[_client.uri().database()]} {}

 private:
  DBConfig _config;
  mongocxx::instance _instance;
  mongocxx::client _client;
  mongocxx::database _connection;
};

using artdaq::database::StorageProvider;

template <typename TYPE>
using MongoDBProvider = StorageProvider<TYPE, MongoDB>;

template <typename TYPE>
using DBProvider = MongoDBProvider<TYPE>;

using DB = MongoDB;

}  // namespace mongo
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_PROVIDER_MONGO_H_ */
