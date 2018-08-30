#ifndef _ARTDAQ_DATABASE_PROVIDER_UCOND_H_
#define _ARTDAQ_DATABASE_PROVIDER_UCOND_H_

#include "artdaq-database/StorageProviders/storage_providers.h"

namespace artdaq {
namespace database {
namespace ucon {
namespace debug {
void enable();
void ReadWrite();
}  // namespace debug

namespace literal {
constexpr auto UCONURI = "http://";
constexpr auto search_index = "index.json";
constexpr auto db_name = "test_db";
constexpr auto hostname = "127.0.0.1";
constexpr auto port = 80;
}  // namespace literal

struct DBConfig final {
  DBConfig();
  DBConfig(const std::string& uri_);

  std::string uri;
  const std::string connectionURI() const { return uri; };
};

class UconDB final {
 public:
  static std::shared_ptr<UconDB> create(DBConfig const& config) { return std::make_shared<UconDB, DBConfig const&, PassKeyIdiom const&>(config, {}); }

  class PassKeyIdiom {
   private:
    friend std::shared_ptr<UconDB> UconDB::create(DBConfig const& config);
    PassKeyIdiom() {}
  };

  explicit UconDB(DBConfig config, PassKeyIdiom const&);

  std::string& connection();
  long timeout() const;
  std::string const& authentication() const;

 private:
  DBConfig _config;
  std::string _client;
  std::string _connection;
  std::string _userpass;
};

using artdaq::database::StorageProvider;

using UconDBSPtr_t = std::shared_ptr<UconDB>;

template <typename TYPE>
using UconDBProvider = StorageProvider<TYPE, UconDB>;

template <typename TYPE>
using DBProvider = UconDBProvider<TYPE>;

using DB = UconDB;

}  // namespace ucon
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_PROVIDER_UCOND_H_ */
