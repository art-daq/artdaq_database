#ifndef _ARTDAQ_DATABASE_PROVIDER_FILESYSTEM_H_
#define _ARTDAQ_DATABASE_PROVIDER_FILESYSTEM_H_

#include "artdaq-database/StorageProviders/storage_providers.h"

#include <vector>
#include <boost/filesystem.hpp>

namespace artdaq {
namespace database {
namespace filesystem {
std::list<std::string> find_subdirs(std::string const&);
std::list<std::string> find_siblingdirs(std::string const& d);
std::list<object_id_t> find_documents(std::string const& d);
std::string mkdir(std::string const& d);
bool check_if_file_exists(std::string const& f);

using file_paths_t = std::vector<boost::filesystem::path>;
file_paths_t list_files_in_directory(boost::filesystem::path const& path, std::string const& ext);
namespace debug {
void enable();
void enableReadWrite();
}

namespace literal {
constexpr auto FILEURI = "filesystemdb://";
constexpr auto search_index = "index.json";
constexpr auto db_name = "test_configuration_db";
}

struct DBConfig final {
  DBConfig();
  DBConfig(std::string uri_);

  std::string uri;
  const std::string connectionURI() const { return uri; };
};

class FileSystemDB final {
 public:
  static std::shared_ptr<FileSystemDB> create(DBConfig const& config) {
    return std::make_shared<FileSystemDB, DBConfig const&, PassKeyIdiom const&>(config, {});
  }

  class PassKeyIdiom {
   private:
    friend std::shared_ptr<FileSystemDB> FileSystemDB::create(DBConfig const& config);
    PassKeyIdiom() {}
  };

  explicit FileSystemDB(DBConfig const& config, PassKeyIdiom const&);

  std::string& connection();

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
