#include "artdaq-database/StorageProviders/common.h"
#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h"

#include "artdaq-database/BasicTypes/basictypes.h"

#include <boost/filesystem.hpp>

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "PRVDR:FileDB_C"

namespace artdaq {
namespace database {

using artdaq::database::basictypes::JsonData;
using artdaq::database::filesystem::FileSystemDB;

void mkdir(std::string const&);

template<>
template <>
std::vector<JsonData> StorageProvider<JsonData, FileSystemDB>::load(JsonData const& filter [[gnu::unused]])
{
    //FIXME:: implement this
    auto returnCollection = std::vector<JsonData>();

    return returnCollection;
}


template<>
object_id_t StorageProvider<JsonData, FileSystemDB>::store(JsonData const& data [[gnu::unused]])
{
    //FIXME:: implement this
    return "error";
}


void mkdir(std::string const& dir)
{
    assert(!dir.empty());
    TRACE_(11, "StorageProvider::FileSystemDB mkdir dir=<" << dir << ">");

    auto path= boost::filesystem::path(dir.c_str());

    if(boost::filesystem::exists(path)) {
        if(boost::filesystem::is_directory(path)) {
            TRACE_(11, "StorageProvider::FileSystemDB Directory exists, checking permissions; path=<" << dir << ">");

            auto mask=boost::filesystem::perms::owner_write|boost::filesystem::perms::owner_read;

            if((boost::filesystem::status(path).permissions() & mask)!=mask)
                TRACE_(11, "StorageProvider::FileSystemDB Directory  <" << dir << "> has wrong permissions; needs to be owner readable and writable");
            throw cet::exception("FileSystemDB") << "Directory  <" << dir << "> has wrong permissions; needs to be owner readable and writable";
        }
        TRACE_(11, "StorageProvider::FileSystemDB Failed creating a directory, sometging in the way path=<" << dir << ">");
        throw cet::exception("FileSystemDB") << "Failed creating a directory, sometging in the way path=<" << dir << ">";
    }

    boost::system::error_code ec;

    if(!boost::filesystem::create_directories(path,ec)) {
        TRACE_(11, "StorageProvider::FileSystemDB Failed creating a directory path=<" << dir << "> error code=" << ec.message());
        throw cet::exception("FileSystemDB") << "Failed creating a directory path=<" << dir << "> error code=" << ec.message();
    }

    auto perms=boost::filesystem::perms::add_perms|boost::filesystem::perms::owner_write|boost::filesystem::perms::owner_read;

    ec.clear();

    boost::filesystem::permissions(path,perms,ec);

    if(ec!= boost::system::errc::success) {
        TRACE_(11, "StorageProvider::FileSystemDB Failed enforcing directory permissions for path=<" << dir << "> error code=" << ec.message() );
        throw cet::exception("FileSystemDB") << "Failed enforcing directory permissions for path=<" << dir << "> error code=" << ec.message();
    }

}

namespace filesystem {
void  trace_enable()
{
    TRACE_CNTL( "name",    TRACE_NAME);
    TRACE_CNTL( "lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL );
    TRACE_CNTL( "modeM", 1LL );
    TRACE_CNTL( "modeS", 1LL );

    TRACE_( 0, "artdaq::database::filesystem::" << "trace_enable");
}
} // namespace filesystem
} //namespace database
} //namespace artdaq
