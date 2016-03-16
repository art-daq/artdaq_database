#ifndef _ARTDAQ_DATABASE_PROVIDER_FILESYSTEM_H_
#define _ARTDAQ_DATABASE_PROVIDER_FILESYSTEM_H_

#include "artdaq-database/StorageProviders/common.h"
#include "artdaq-database/StorageProviders/storage_providers.h"

namespace artdaq {
namespace database {
namespace filesystem {

void trace_enable();

namespace literal {
constexpr auto FILEURI = "file://";
constexpr auto search_index = "index.json";
}

struct DBConfig final {
  std::string path = "${HOME}/databases/filesystemdb";
  std::string db_name = "test_configuration_db";
  const std::string connectionURI() const { return std::string{literal::FILEURI} + path; };
};

class FileSystemDB final {
 public:
  auto& connection() { return _connection; }

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
