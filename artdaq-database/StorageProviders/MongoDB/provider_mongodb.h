#ifndef _ARTDAQ_DATABASE_PROVIDER_MONGO_H_
#define _ARTDAQ_DATABASE_PROVIDER_MONGO_H_

#include "artdaq-database/StorageProviders/common.h"
#include "artdaq-database/StorageProviders/storage_providers.h"

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

namespace artdaq {
namespace database {
namespace mongo {

void trace_enable();

struct DBConfig final {
  std::string hostname = "127.0.0.1";
  unsigned int port = 27017;
  std::string db_name = "test_configuration_db";
  const std::string connectionURI() const { return "mongodb://" + hostname + ":" + std::to_string(port); };
};

class MongoDB final {
 public:
  auto& connection() { return _connection; }

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
        _connection{_client[_config.db_name]} {}

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
