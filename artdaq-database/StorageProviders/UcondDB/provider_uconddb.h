#ifndef _ARTDAQ_DATABASE_PROVIDER_UCOND_H_
#define _ARTDAQ_DATABASE_PROVIDER_UCOND_H_

#include "artdaq-database/StorageProviders/common.h"
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
  DBConfig() : uri{std::string{literal::FILEURI} + "${ARTDAQ_DATABASE_DATADIR}/uconddb" + "/" + literal::db_name} {
    auto tmpURI =
        getenv("ARTDAQ_DATABASE_URI") ? expand_environment_variables("${ARTDAQ_DATABASE_URI}") : std::string("");

    if (tmpURI.back() == '/') tmpURI.pop_back();  // remove trailing slash

    auto prefixURI = std::string{literal::FILEURI};

    if (tmpURI.length() > prefixURI.length() && std::equal(prefixURI.begin(), prefixURI.end(), tmpURI.begin()))
      uri = tmpURI;
  }

  DBConfig(std::string uri_) : uri{uri_} { assert(!uri_.empty()); }
  std::string uri;
  const std::string connectionURI() const { return uri; };
};

class UcondDB final {
 public:
  std::string& connection();

  static std::shared_ptr<UcondDB> create(DBConfig const& config) {
    return std::make_shared<UcondDB, DBConfig const&, PassKeyIdiom const&>(config, {});
  }

  class PassKeyIdiom {
   private:
    friend std::shared_ptr<UcondDB> UcondDB::create(DBConfig const& config);
    PassKeyIdiom() {}
  };

  explicit UcondDB(DBConfig const& config, PassKeyIdiom const&)
      : _config{config}, _client{_config.connectionURI()}, _connection{_config.connectionURI() + "/"} {}

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
