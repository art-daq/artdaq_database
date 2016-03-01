#include "artdaq-database/StorageProviders/common.h"
#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h"
#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb_index.h"

#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"

#include "artdaq-database/BasicTypes/basictypes.h"
#include <wordexp.h>
#include <boost/filesystem.hpp>

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "PRVDR:FileDB_C"

namespace dbfsl =artdaq::database::filesystem::literal;

namespace artdaq {
namespace database {

using artdaq::database::basictypes::JsonData;
using artdaq::database::filesystem::FileSystemDB;
using artdaq::database::filesystem::index::SearchIndex;

using artdaq::database::jsonutils::JSONDocumentBuilder;

std::string mkdir(std::string const&);
object_id_t generate_oid();
std::string expand_environment_variables(std::string var);

template<>
template <>
std::vector<JsonData> StorageProvider<JsonData, FileSystemDB>::load(JsonData const& filter)
{
    TRACE_(3, "StorageProvider::FileSystemDB::load() begin");
    TRACE_(3, "StorageProvider::FileSystemDB::load() args search=<" << filter.json_buffer << ">");

    auto builder = JSONDocumentBuilder {filter.json_buffer};

    auto collection_name = builder.extract().findChild("collection").value();
    auto search_filter = builder.extract().findChild("filter").value();

    auto collection = _provider->connection().append(collection_name);
    collection = expand_environment_variables(collection);

    TRACE_(4, "StorageProvider::FileSystemDB::load() collection_path=<" << collection << ">." );

    auto dir_name =mkdir(collection);

    auto index_path= boost::filesystem::path(dir_name.c_str()).append(dbfsl::search_index);

    SearchIndex search_index(index_path);

    auto oids = search_index.findDocumentIDs(search_filter);

    auto returnCollection = std::vector<JsonData>();
    returnCollection.reserve(oids.size());
    TRACE_(4, "StorageProvider::FileSystemDB::load() search returned " << oids.size() << " documents." );

    for(auto const& oid : oids) {

        auto doc_path= boost::filesystem::path(dir_name.c_str()).append(oid).replace_extension(".json");

        TRACE_(4, "StorageProvider::FileSystemDB::load() reading document <" << doc_path.c_str() << ">." );

        std::ifstream is(doc_path.c_str());
        std::string json((std::istreambuf_iterator<char>(is)),
                         std::istreambuf_iterator<char>());

        is.close();
        returnCollection.push_back( {json});
    }

    return returnCollection;
}


template<>
object_id_t StorageProvider<JsonData, FileSystemDB>::store(JsonData const& data)
{
    TRACE_(4, "StorageProvider::FileSystemDB::store() begin");

    auto builder = JSONDocumentBuilder {data.json_buffer};

    auto oid=generate_oid();
    TRACE_(4, "StorageProvider::FileSystemDB::store() oid=<" << oid << ">." );

    std::stringstream ss;

    ss << "{ \"_id\":\"" << oid << "\"}";

    //builder.setObjectID( {ss.str()} );

    TRACE_(4, "StorageProvider::FileSystemDB::store() document=<" << builder.extract() << ">." );

    auto collection_name = builder.extract().findChild("collection").value();


    auto collection = _provider->connection().append(collection_name);

    collection = expand_environment_variables(collection);

    TRACE_(4, "StorageProvider::FileSystemDB::store() collection_path=<" << collection << ">." );

    auto filename= mkdir(collection) +oid + ".json";

    TRACE_(4, "StorageProvider::FileSystemDB::store() filename=<" << filename << ">." );

    std::ofstream os(filename);

    auto json = builder.extract().findChild("document").value();

    std::copy(json.begin(),json.end(),std::ostream_iterator<char>(os));

    os.close();

    auto index_path= boost::filesystem::path(filename.c_str()).parent_path().append(dbfsl::search_index);

    SearchIndex search_index(index_path);

    if(!search_index.addDocument(json, oid)) {
        TRACE_(4, "StorageProvider::FileSystemDB Failed updating SearchIndex." );
    }

    return oid;
}


std::string mkdir(std::string const& d)
{
    assert(!d.empty());

    auto dir = d;

    if(dir.find(dbfsl::FILEURI)==0)
        dir  = dir.substr( strlen(dbfsl::FILEURI));

    TRACE_(11, "StorageProvider::FileSystemDB mkdir dir=<" << dir << ">");

    auto path= boost::filesystem::path(dir.c_str());

    if(boost::filesystem::exists(path)) {
        if(boost::filesystem::is_directory(path)) {
            TRACE_(11, "StorageProvider::FileSystemDB Directory exists, checking permissions; path=<" << dir << ">");

            auto mask=boost::filesystem::perms::owner_write|boost::filesystem::perms::owner_read;

            if((boost::filesystem::status(path).permissions() & mask)!=mask) {
                TRACE_(11, "StorageProvider::FileSystemDB Directory  <" << dir << "> has wrong permissions; needs to be owner readable and writable");
                throw cet::exception("FileSystemDB") << "Directory  <" << dir << "> has wrong permissions; needs to be owner readable and writable";
            }
        } else {
            TRACE_(11, "StorageProvider::FileSystemDB Failed creating a directory, sometging in the way path=<" << dir << ">");
            throw cet::exception("FileSystemDB") << "Failed creating a directory, sometging in the way path=<" << dir << ">";
        }
    }

    boost::system::error_code ec;

    if(!boost::filesystem::create_directories(path,ec)) {
        if(ec!=boost::system::errc::success) {
            TRACE_(11, "StorageProvider::FileSystemDB Failed creating a directory path=<" << dir << "> error code=" << ec.message());
            throw cet::exception("FileSystemDB") << "Failed creating a directory path=<" << dir << "> error code=" << ec.message();
        }
    }

    auto perms=boost::filesystem::perms::add_perms|boost::filesystem::perms::owner_write|boost::filesystem::perms::owner_read;

    ec.clear();

    boost::filesystem::permissions(path,perms,ec);

    if(ec!= boost::system::errc::success) {
        TRACE_(11, "StorageProvider::FileSystemDB Failed enforcing directory permissions for path=<" << dir << "> error code=" << ec.message() );
        throw cet::exception("FileSystemDB") << "Failed enforcing directory permissions for path=<" << dir << "> error code=" << ec.message();
    }

    return dir;
}


object_id_t generate_oid() {
    std::ifstream is("/proc/sys/kernel/random/uuid");

    std::string oid((std::istreambuf_iterator<char>(is)),
                    std::istreambuf_iterator<char>());

    oid.erase (std::remove(oid.begin(), oid.end(), '-'), oid.end());
    oid.resize(24);

    TRACE_(12, "StorageProvider::FileSystemDB generate_oid=" <<oid );

    return oid;
}

std::string expand_environment_variables(std::string var) {
    wordexp_t p;
    char** w;

    ::wordexp(var.c_str(), &p, 0 );

    w = p.we_wordv;

    std::stringstream ss;

    for (size_t i=0; i<p.we_wordc; i++ )
        ss << w[i] << "/";

    ::wordfree( &p );

    return ss.str();
}

namespace filesystem {
void  trace_enable()
{
    TRACE_CNTL( "name",    TRACE_NAME);
    TRACE_CNTL( "lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL );
    TRACE_CNTL( "modeM", 1LL );
    TRACE_CNTL( "modeS", 1LL );

    TRACE_( 0, "artdaq::database::filesystem::" << "trace_enable");

    index::trace_enable();
}
} // namespace filesystem
} //namespace database
} //namespace artdaq
