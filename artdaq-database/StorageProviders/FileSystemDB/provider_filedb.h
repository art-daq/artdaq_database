#ifndef _ARTDAQ_DATABASE_PROVIDER_FILESYSTEM_H_
#define _ARTDAQ_DATABASE_PROVIDER_FILESYSTEM_H_

#include "artdaq-database/StorageProviders/common.h"
#include "artdaq-database/StorageProviders/storage_providers.h"

#ifdef TRACE_NAME
  #undef TRACE_NAME
#endif

#define TRACE_NAME "PRVDR:FileDB_H"

namespace artdaq{
namespace database{
namespace filesystem{

void  trace_enable();
  
struct DBConfig final {
    std::string hostname = "127.0.0.1";
    std::string path = "path";
    std::string db_name = "test_configuration_db";
    const std::string connectionURI() const {
        return "file://" + hostname + "/" + path;
    };
};


class FileSystemDB final
{

public:
    auto& connection() {
        return _connection;
    }

    static std::shared_ptr<FileSystemDB> create(DBConfig const& config) {
        return std::make_shared<FileSystemDB, DBConfig const&, PassKeyIdiom const&>(config, {});
    }

    class PassKeyIdiom
    {
    private:
        friend std::shared_ptr<FileSystemDB> FileSystemDB::create(DBConfig const& config);
        PassKeyIdiom() {}
    };

    explicit FileSystemDB(DBConfig const& config, PassKeyIdiom const&):
        _config {config},
        _client {_config.connectionURI()},
        _connection{_config.connectionURI()}{
    }


private:
    DBConfig _config;
    std::string _client;
    std::string _connection;

};

using  artdaq::database::StorageProvider;

template <typename TYPE>
using  FileSystemDBProvider = StorageProvider<TYPE,FileSystemDB>;

template <typename TYPE>
using DBProvider = FileSystemDBProvider<TYPE>;

using DB = FileSystemDB;

} // namespace mongo
} //namespace database
} //namespace artdaq

#endif /* _ARTDAQ_DATABASE_PROVIDER_FILESYSTEM_H_ */
