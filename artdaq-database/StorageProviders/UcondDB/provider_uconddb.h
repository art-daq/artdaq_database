#ifndef _ARTDAQ_DATABASE_PROVIDER_UCOND_H_
#define _ARTDAQ_DATABASE_PROVIDER_UCOND_H_

#include "artdaq-database/StorageProviders/storage_providers.h"

namespace artdaq {
namespace database {
namespace ucond {
namespace debug {
void enable();
}

namespace literal {
constexpr auto FILEURI = "uconddb://";
constexpr auto search_index = "index.json";
constexpr auto db_name = "test_configuration_db";
}

struct DBConfig final {
  DBConfig();
  DBConfig(std::string uri_);

  std::string uri;
  const std::string connectionURI() const { return uri; };
};

class UcondDB final {
 public:
  static std::shared_ptr<UcondDB> create(DBConfig const& config) {
    return std::make_shared<UcondDB, DBConfig const&, PassKeyIdiom const&>(config, {});
  }

  class PassKeyIdiom {
   private:
    friend std::shared_ptr<UcondDB> UcondDB::create(DBConfig const& config);
    PassKeyIdiom() {}
  };
  
  explicit UcondDB(DBConfig const& config, PassKeyIdiom const&);

  std::string& connection();

 private:
  DBConfig _config;
  std::string _client;
  std::string _connection;
};

using artdaq::database::StorageProvider;

template <typename TYPE>
using UcondDBProvider = StorageProvider<TYPE, UcondDB>;

template <typename TYPE>
using DBProvider = UcondDBProvider<TYPE>;

using DB = UcondDB;

}  // namespace ucond
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_PROVIDER_UCOND_H_ */
