#ifndef _ARTDAQ_DATABASE_PROVIDER_FILESYSTEM_H_
#define _ARTDAQ_DATABASE_PROVIDER_FILESYSTEM_H_

#include "artdaq-database/StorageProviders/common.h"
#include "artdaq-database/StorageProviders/storage_providers.h"

namespace artdaq {
namespace database {
namespace filesystem {
namespace debug {
void enable();
}

namespace literal {
constexpr auto FILEURI = "filesystemdb://";
constexpr auto search_index = "index.json";
constexpr auto db_name = "test_configuration_db";
}

struct DBConfig final {
  DBConfig() : uri{std::string{literal::FILEURI} + "${ARTDAQ_DATABASE_DATADIR}/filesystemdb" + "/" + literal::db_name} {
    auto tmpURI = getenv("ARTDAQ_DATABASE_URI") ? expand_environment_variables("${ARTDAQ_DATABASE_URI}") : std::string("");

    if (tmpURI.back() == '/') tmpURI.pop_back();  // remove trailing slash

    auto prefixURI = std::string{literal::FILEURI};

    if (tmpURI.length() > prefixURI.length() && std::equal(prefixURI.begin(), prefixURI.end(), tmpURI.begin())) uri = tmpURI;
  }

  DBConfig(std::string uri_) : uri{uri_} { assert(!uri_.empty()); }
  std::string uri;
  const std::string connectionURI() const { return uri; };
};

class FileSystemDB final {
 public:
  std::string& connection();

  static std::shared_ptr<FileSystemDB> create(DBConfig const& config) {
    return std::make_shared<FileSystemDB, DBConfig const&, PassKeyIdiom const&>(config, {});
  }

  class PassKeyIdiom {
   private:
    friend std::shared_ptr<FileSystemDB> FileSystemDB::create(DBConfig const& config);
    PassKeyIdiom() {}
  };

  explicit FileSystemDB(DBConfig const& config, PassKeyIdiom const&)
      : _config{config}, _client{_config.connectionURI()}, _connection{_config.connectionURI() + "/"} {}

 private:
  DBConfig _config;
  std::string _client;
  std::string _connection;
};

using artdaq::database::StorageProvider;

template <typename TYPE>
using FileSystemDBProvider = StorageProvider<TYPE, FileSystemDB>;

template <typename TYPE>
using DBProvider = FileSystemDBProvider<TYPE>;

using DB = FileSystemDB;

}  // namespace filesystem
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_PROVIDER_FILESYSTEM_H_ */
