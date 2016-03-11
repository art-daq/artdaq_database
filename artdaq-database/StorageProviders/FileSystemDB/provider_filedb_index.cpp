#include "artdaq-database/StorageProviders/common.h"
#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb_index.h"

#include <fstream>
#include "artdaq-database/FhiclJson/json_common.h"
#include "artdaq-database/BasicTypes/data_json.h"

#include <boost/exception/diagnostic_information.hpp>
#include <boost/filesystem.hpp>

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "PRVDR:FileDBIX_C"

namespace dbfsi =artdaq::database::filesystem::index;
using namespace artdaq::database::filesystem::index;

using file_paths_t = std::vector<boost::filesystem::path>;
file_paths_t list_files_in_directory(boost::filesystem::path const&,std::string const&);


namespace jsn = artdaq::database::json;

using jsn::JsonReader;
using jsn::JsonWriter;

using artdaq::database::sharedtypes::unwrap;


typedef std::vector<object_id_t> (SearchIndex::* matching_function_t)(std::string const&);


SearchIndex::SearchIndex(boost::filesystem::path const& path)
    : _index {}, _path {path}, _isOpen {_open(path)} {

}

SearchIndex::~SearchIndex() {
    try {
        _close();
    } catch(...) {
        TRACE_(10, "Exception in StorageProvider::FileSystemDB::~SearchIndex() "
               << boost::current_exception_diagnostic_information());
    }
}

std::vector<object_id_t> SearchIndex::findDocumentIDs(JsonData const& search)
{
    auto ouids = std::vector<object_id_t> {};

    assert(!search.json_buffer.empty());

    TRACE_(2, "StorageProvider::FileSystemDB::index::findDocumentIDs() begin");
    TRACE_(2, "StorageProvider::FileSystemDB::index::findDocumentIDs() args search=<" << search.json_buffer << ">.");

    auto reader=JsonReader {};

    object_t  search_ast;

    if(!reader.read(search.json_buffer, search_ast)) {
        TRACE_(5, "StorageProvider::FileSystemDB::index::findDocumentIDs() Failed to create an AST from search.");
        return ouids;
    }

    TRACE_(5, "StorageProvider::FileSystemDB::index::findDocumentIDs() found " << search_ast.size() << " keys.");


    auto runMatchingFunction = [](std::string const & name) {
        auto matching_function_map = std::map<std::string, matching_function_t> {
            {"version", &SearchIndex::_matchVersion}
            ,{"configurations.name", &SearchIndex::_matchConfiguration}
            ,{"configurable_entity.name", &SearchIndex::_matchConfigurableEntity}
            ,{"$oid", &SearchIndex::_matchObjectId}
            ,{"_id", &SearchIndex::_matchObjectIds}
        };

        return matching_function_map.at(name);
    };


    ouids.reserve(128);

    for(auto const& search_criterion : search_ast) {
	if(search_criterion.value.type() == typeid(std::string)){      
	  auto const& value=boost::get<std::string>(search_criterion.value);
	  auto matches = (this->*runMatchingFunction(search_criterion.key))(value);
	  ouids.insert( ouids.end(), matches.begin(), matches.end() );
	}else if ( search_criterion.value.type() == typeid(jsn::object_t)) {
	  auto const& value=boost::get<jsn::object_t>(search_criterion.value);
	  auto writer=JsonWriter{};
	  auto json=std::string();  
	  if(!writer.write(value,json)) {
	      TRACE_(5, "StorageProvider::FileSystemDB::index::findDocumentIDs() Failed to write an AST to json, key=<" << search_criterion.key << ">." );
	      throw cet::exception("FileSystemDB") << "StorageProvider::FileSystemDB::index::findDocumentIDs() Failed to write an AST to json.";
	  }
	  
	  auto matches = (this->*runMatchingFunction(search_criterion.key))(json);
	  
	  ouids.insert( ouids.end(), matches.begin(), matches.end() );	  
	  
	  
	}
    }

    if(ouids.size()<2)
      return ouids;
    
    auto unique_ouids = std::vector<object_id_t> {};
    std::unique_copy(ouids.begin(), ouids.end(), back_inserter(unique_ouids));     
    auto nonUnique_ouids = std::vector<object_id_t> {};
    std::set_difference(ouids.begin(), ouids.end(), unique_ouids.begin(), unique_ouids.end(), back_inserter(nonUnique_ouids));
    
    ouids.swap(nonUnique_ouids);
     
    return ouids;
}

bool SearchIndex::addDocument(JsonData const& document,object_id_t const& ouid)
{
    assert(!document.json_buffer.empty());
    assert(!ouid.empty());

    TRACE_(5, "StorageProvider::FileSystemDB::index::addDocument() begin");
    TRACE_(5, "StorageProvider::FileSystemDB::index::addDocument() args document=<" << document.json_buffer << ">.");
    TRACE_(5, "StorageProvider::FileSystemDB::index::addDocument() args ouid=<" << ouid << ">.");

    auto reader=JsonReader {};

    object_t  doc_ast;

    if(!reader.read(document.json_buffer, doc_ast)) {
        TRACE_(5, "StorageProvider::FileSystemDB::index::addDocument() Failed to create an AST from document.");
        return false;
    }

    TRACE_(5, "StorageProvider::FileSystemDB::index::addDocument() found " << doc_ast.size() << " keys.");

    try {
        auto version = boost::get<std::string>(doc_ast.at("version"));

        _addVersion(ouid,version);

        auto configs = boost::get<jsn::array_t>(doc_ast.at("configurations"));

        for(auto const& config : configs) {
            auto configuration=boost::get<std::string>(boost::get<jsn::object_t>(config).at("name"));
            _addConfiguration(ouid,configuration);
        }

        try {
             auto configurable_entity = boost::get<object_t>(doc_ast.at("configurable_entity"));
             auto entity_name = boost::get<std::string>(configurable_entity.at("name"));

            _addConfigurableEntity(ouid,entity_name);

        } catch(...) {
	      TRACE_(5, "StorageProvider::FileSystemDB::index::addDocument() Failed to add configurable_entity.");
	}

        return true;

    } catch(...) {

        for(auto const&kvp :  doc_ast) {
            TRACE_(5, "StorageProvider::FileSystemDB::index::addDocument() key=<" << kvp.key<< ">");
        }

        TRACE_(5, "Exception in StorageProvider::FileSystemDB::index::addDocument() "
               << boost::current_exception_diagnostic_information());
    }

    return false;
}

bool SearchIndex::removeDocument(JsonData const& document, object_id_t const& ouid)
{
    assert(!document.json_buffer.empty());
    assert(!ouid.empty());

    TRACE_(5, "StorageProvider::FileSystemDB::index::removeDocument() begin");
    TRACE_(5, "StorageProvider::FileSystemDB::index::removeDocument() args document=<" << document.json_buffer << ">.");
    TRACE_(5, "StorageProvider::FileSystemDB::index::removeDocument() args ouid=<" << ouid << ">.");

    auto reader=JsonReader {};

    object_t  doc_ast;

    if(!reader.read(document.json_buffer, doc_ast)) {
        TRACE_(6, "StorageProvider::FileSystemDB::index::removeDocument() Failed to create an AST from document.");
        return false;
    }

    TRACE_(5, "StorageProvider::FileSystemDB::index::removeDocument() found " << doc_ast.size() << " keys.");

    try {
        auto version = boost::get<std::string>(doc_ast.at("version"));

        _removeVersion(ouid,version);

        auto configs = boost::get<jsn::array_t>(doc_ast.at("configurations"));

        for(auto const& config : configs) {
            auto configuration=boost::get<std::string>(boost::get<jsn::object_t>(config).at("name"));
            _removeConfiguration(ouid,configuration);
        }

        return true;

    } catch(...) {
        for(auto const&kvp :  doc_ast) {
            TRACE_(5, "StorageProvider::FileSystemDB::index::addDocument() key=<" << kvp.key<< ">");
        }

        TRACE_(6, "Exception in StorageProvider::FileSystemDB::index::removeDocument() "
               << boost::current_exception_diagnostic_information());
    }

    return false;
}

bool SearchIndex::_create(boost::filesystem::path const& index_path) {
    try {
        std::ofstream os(index_path.c_str());

        auto json = std::string {
            "{\"version\":{},\"configurations.name\":{}, \"configurable_entity.name\":{} }"
        };

        std::copy(json.begin(),json.end(),std::ostream_iterator<char>(os));

        os.close();

        return true;
    } catch(...) {
        TRACE_(11, "Exception in StorageProvider::FileSystemDB::SearchIndex::_create() "
               << boost::current_exception_diagnostic_information());
        return false;
    }
}

bool SearchIndex::_open(boost::filesystem::path const& index_path) {
    TRACE_(3, "StorageProvider::FileSystemDB::index::_open() begin");
    TRACE_(3, "StorageProvider::FileSystemDB::index::_open() args index_path=<" << index_path.c_str() << ">.");

    if(!_index.empty()) {
        TRACE_(3, "StorageProvider::FileSystemDB::SearchIndex SearchIndex was aready opened, path=<" << index_path.c_str() << ">");
        throw cet::exception("FileSystemDB") << "StorageProvider::FileSystemDB::SearchIndex SearchIndex was aready opened, path=<" << index_path.c_str() << ">"	;
    }

    if(!boost::filesystem::exists(index_path) && !_create(index_path)) {
        TRACE_(3, "StorageProvider::FileSystemDB::SearchIndex Failed creating SearchIndex, path=<" << index_path.c_str() << ">");
        throw cet::exception("FileSystemDB") << "StorageProvider::FileSystemDB::SearchIndex Failed creating SearchIndex, path=<" << index_path.c_str() << ">";
    }

    std::ifstream is(index_path.c_str());

    std::string json((std::istreambuf_iterator<char>(is)),
                     std::istreambuf_iterator<char>());

    is.close();

    TRACE_(3, "StorageProvider::FileSystemDB::index::_open() json=<" << json << ">");

    auto reader=JsonReader {};

    if(reader.read(json, _index))
        return true;

    TRACE_(3, "StorageProvider::FileSystemDB::index::_open() Failed to open SearchIndex.");

    return false;
}

bool SearchIndex::_close() {
    if(!_isOpen)
        return true;

    TRACE_(4, "StorageProvider::FileSystemDB::index::_close() begin");

    auto writer=JsonWriter {};

    auto json = std::string();

    if(!writer.write(_index,json)) {
        TRACE_(4, "StorageProvider::FileSystemDB::index::_close() Failed to close SearchIndex.");
        return false;
    }

    TRACE_(4, "StorageProvider::FileSystemDB::index::_close() json=<" << json << ">");

    try {
        std::ofstream os(_path.c_str());

        std::copy(json.begin(),json.end(),std::ostream_iterator<char>(os));

        os.close();

        TRACE_(4, "StorageProvider::FileSystemDB::index::_close() Closed SearchIndex.");

        return true;
    } catch(...) {
        TRACE_(4, "StorageProvider::FileSystemDB::index::_close() Failed to write SearchIndex; error:"
               << boost::current_exception_diagnostic_information());
        return false;
    }
}


void  SearchIndex::_addVersion(object_id_t const & ouid,std::string const& version)
{
    assert(!ouid.empty());
    assert(!version.empty());

    TRACE_(7, "StorageProvider::FileSystemDB::index::_addVersion() begin");
    TRACE_(7, "StorageProvider::FileSystemDB::index::_addVersion() args ouid=<" << ouid<< ">.");
    TRACE_(7, "StorageProvider::FileSystemDB::index::_addVersion() args version=<" << version<< ">.");

    auto& versions = boost::get<jsn::object_t>(_index.at("version"));

    auto& version_ouid_list=versions[version];

    if(version_ouid_list.type() == typeid(bool))
        version_ouid_list=jsn::array_t {};

    boost::get<jsn::array_t>(version_ouid_list).push_back(ouid);
}

void  SearchIndex::_addConfiguration(object_id_t const& ouid,std::string const& configuration) {
    assert(!ouid.empty());
    assert(!configuration.empty());

    TRACE_(7, "StorageProvider::FileSystemDB::index::_addConfiguration() begin");
    TRACE_(7, "StorageProvider::FileSystemDB::index::_addConfiguration() args ouid=<" << ouid<< ">.");
    TRACE_(7, "StorageProvider::FileSystemDB::index::_addConfiguration() args configuration=<" << configuration<< ">.");

    auto& configurations = boost::get<jsn::object_t>(_index.at("configurations.name"));
    auto& configuration_ouid_list=configurations[configuration];

    if(configuration_ouid_list.type() == typeid(bool))
        configuration_ouid_list=jsn::array_t {};

    boost::get<jsn::array_t>(configuration_ouid_list).push_back(ouid);
}

void  SearchIndex::_addConfigurableEntity(object_id_t const& ouid,std::string const& entity) {
    assert(!ouid.empty());
    assert(!entity.empty());

    TRACE_(7, "StorageProvider::FileSystemDB::index::_addConfigurableEntity() begin");
    TRACE_(7, "StorageProvider::FileSystemDB::index::_addConfigurableEntity() args ouid=<" << ouid<< ">.");
    TRACE_(7, "StorageProvider::FileSystemDB::index::_addConfigurableEntity() args entity=<" << entity<< ">.");

    auto& entities = boost::get<jsn::object_t>(_index.at("configurable_entity.name"));
    auto& entity_ouid_list=entities[entity];

    if(entity_ouid_list.type() == typeid(bool))
        entity_ouid_list=jsn::array_t {};

    boost::get<jsn::array_t>(entity_ouid_list).push_back(ouid);
}

std::vector<object_id_t>  SearchIndex::_matchVersion(std::string const& version) {
    auto ouids= std::vector<object_id_t> {};

    assert(!version.empty());

    TRACE_(14, "StorageProvider::FileSystemDB::index::_matchVersion() begin");
    TRACE_(14, "StorageProvider::FileSystemDB::index::_matchVersion() args version=<" << version<< ">.");

    auto& versions = boost::get<jsn::object_t>(_index.at("version"));

    try {
        auto& version_ouid_list = boost::get<jsn::array_t>(versions.at(version));

        ouids.reserve(version_ouid_list.size());

        for(auto& version_ouid :version_ouid_list) {
            ouids.push_back(unwrap(version_ouid).value_as<std::string>());
        }
        TRACE_(15, "StorageProvider::FileSystemDB::index::_matchConfiguration() Found " << ouids.size() << " documents where version=" << version );
        
    } catch(std::out_of_range const &) {
        TRACE_(14, "StorageProvider::FileSystemDB::index::_matchVersion() Version not found, version=<" << version<< ">.");
    }

    return ouids;
}

std::vector<object_id_t>  SearchIndex::_matchConfiguration(std::string const& configuration) {
    auto ouids= std::vector<object_id_t> {};

    assert(!configuration.empty());

    TRACE_(15, "StorageProvider::FileSystemDB::index::_matchConfiguration() begin");
    TRACE_(15, "StorageProvider::FileSystemDB::index::_matchConfiguration() args configuration=<" << configuration<< ">.");

    auto& versions = boost::get<jsn::object_t>(_index.at("configurations.name"));

    try {
        auto& configuration_ouid_list = boost::get<jsn::array_t>(versions.at(configuration));

        ouids.reserve(configuration_ouid_list.size());

        for(auto& configuration_ouid :configuration_ouid_list) {
            ouids.push_back(unwrap(configuration_ouid).value_as<std::string>());
        }
        TRACE_(15, "StorageProvider::FileSystemDB::index::_matchConfiguration() Found " << ouids.size() << " documents where configuration=" << configuration );
        
    } catch(std::out_of_range const &) {
        TRACE_(15, "StorageProvider::FileSystemDB::index::_matchConfiguration() Configuration not found, configuration=<" << configuration<< ">.");
    }

    return ouids;
}


std::vector<object_id_t>  SearchIndex::_matchConfigurableEntity(std::string const& entity){
    auto ouids= std::vector<object_id_t> {};

    assert(!entity.empty());

    TRACE_(15, "StorageProvider::FileSystemDB::index::_matchConfigurableEntity() begin");
    TRACE_(15, "StorageProvider::FileSystemDB::index::_matchConfigurableEntity() args configurable_entity=<" << entity<< ">.");

    auto& entities = boost::get<jsn::object_t>(_index.at("configurable_entity.name"));

    try {
        auto& entity_ouid_list = boost::get<jsn::array_t>(entities.at(entity));

        ouids.reserve(entity_ouid_list.size());

        for(auto& entity_ouid :entity_ouid_list) {
            ouids.push_back(unwrap(entity_ouid).value_as<std::string>());
        }
        TRACE_(15, "StorageProvider::FileSystemDB::index::_matchConfigurableEntity() Found " << ouids.size() << " documents where entity=" << entity );
    } catch(std::out_of_range const &) {
        TRACE_(15, "StorageProvider::FileSystemDB::index::_matchConfigurableEntity() Configuration entity not found, entity=<" << entity<< ">.");
    }

    return ouids;
}

void  SearchIndex::_removeVersion(object_id_t const& ouid,std::string const& version)
{
    assert(!ouid.empty());
    assert(!version.empty());

    TRACE_(7, "StorageProvider::FileSystemDB::index::_removeVersion() begin");
    TRACE_(7, "StorageProvider::FileSystemDB::index::_removeVersion() args ouid=<" << ouid<< ">.");
    TRACE_(7, "StorageProvider::FileSystemDB::index::_removeVersion() args version=<" << version<< ">.");

    auto& versions = boost::get<jsn::object_t>(_index.at("version"));

    try {
        auto& version_ouid_list = boost::get<jsn::array_t>(versions.at(version));
        auto new_version_ouid_list=jsn::array_t {};

        for(auto& version_ouid :version_ouid_list) {
            auto this_ouid=unwrap(version_ouid).value_as<std::string>();

            if(this_ouid!=ouid) {
                new_version_ouid_list.push_back(this_ouid);
            } else {
                TRACE_(7, "StorageProvider::FileSystemDB::index::_removeVersion() removed ouid=<" << ouid<< ">.");
            }
        }
    } catch(std::out_of_range const &) {
        TRACE_(7, "StorageProvider::FileSystemDB::index::_removeVersion() Version not found, version=<" << version<< ">.");
    }
}

std::vector<object_id_t>  SearchIndex::_matchObjectId(std::string const& objectid){
    auto ouids= std::vector<object_id_t> {};

    assert(!objectid.empty());

    TRACE_(16, "StorageProvider::FileSystemDB::index::_matchObjectId() begin");
    TRACE_(16, "StorageProvider::FileSystemDB::index::_matchObjectId() args objectid=<" << objectid<< ">.");
    
    ouids.push_back(objectid);
    
    return ouids;
}

std::vector<object_id_t>  SearchIndex::_matchObjectIds(std::string const& objectids){
    auto ouids= std::vector<object_id_t> {};

    assert(!objectids.empty());

    TRACE_(16, "StorageProvider::FileSystemDB::index::_matchObjectIds() begin");
    TRACE_(16, "StorageProvider::FileSystemDB::index::_matchObjectIds() args objectid=<" << objectids<< ">.");
    
    auto reader=JsonReader {};

    object_t  objectids_ast;

    if(!reader.read(objectids, objectids_ast)) {
        TRACE_(16, "StorageProvider::FileSystemDB::index::_matchObjectIds() Failed to create an AST from objectids.");
        return ouids;
    }

    auto ouid_list = boost::get<jsn::array_t>(objectids_ast.at("$in"));

    TRACE_(5, "StorageProvider::FileSystemDB::index::_matchObjectIds() found " << ouid_list.size() << " ouids.");
    
    for(auto& ouid_entry :ouid_list) {      
	 auto const& ouid= boost::get<std::string>(boost::get<jsn::object_t>(ouid_entry).at("$oid"));	 
	 TRACE_(16, "StorageProvider::FileSystemDB::index::_matchObjectIds() found ouid=<" <<  ouid << ">.");
	 ouids.push_back(ouid);
    }

    TRACE_(15, "StorageProvider::FileSystemDB::index::_matchObjectIds() Found " << ouids.size() << " documents" );
    
    return ouids;
}
void  SearchIndex::_removeConfiguration(object_id_t const& ouid,std::string const& configuration) {
    assert(!ouid.empty());
    assert(!configuration.empty());

    TRACE_(8, "StorageProvider::FileSystemDB::index::_removeConfiguration() begin");
    TRACE_(8, "StorageProvider::FileSystemDB::index::_removeConfiguration() args ouid=<" << ouid<< ">.");
    TRACE_(8, "StorageProvider::FileSystemDB::index::_removeConfiguration() args configuration=<" << configuration<< ">.");

    auto& configurations = boost::get<jsn::object_t>(_index.at("configurations.name"));

    try {
        auto& configuration_ouid_list = boost::get<jsn::array_t>(configurations.at(configuration));
        auto new_configuration_ouid_list=jsn::array_t {};

        for(auto& configuration_ouid :configuration_ouid_list) {
            auto this_ouid=unwrap(configuration_ouid).value_as<std::string>();

            if(this_ouid!=ouid) {
                new_configuration_ouid_list.push_back(this_ouid);
            } else {
                TRACE_(8, "StorageProvider::FileSystemDB::index::_removeConfiguration() removed ouid=<" << ouid<< ">.");
            }
        }
    } catch(std::out_of_range const &) {
        TRACE_(8, "StorageProvider::FileSystemDB::index::_removeConfiguration() Configuration not found, configuration=<" << configuration<< ">.");
    }
}

void  SearchIndex::_removeConfigurableEntity(object_id_t const& ouid,std::string const& entity) {
    assert(!ouid.empty());
    assert(!entity.empty());

    TRACE_(8, "StorageProvider::FileSystemDB::index::_removeConfigurableEntity() begin");
    TRACE_(8, "StorageProvider::FileSystemDB::index::_removeConfigurableEntity() args ouid=<" << ouid<< ">.");
    TRACE_(8, "StorageProvider::FileSystemDB::index::_removeConfigurableEntity() args entity=<" << entity<< ">.");

    auto& entities = boost::get<jsn::object_t>(_index.at("configurable_entity.name"));

    try {
        auto& entity_ouid_list = boost::get<jsn::array_t>(entities.at(entity));
        auto new_entity_ouid_list=jsn::array_t {};

        for(auto& entity_ouid :entity_ouid_list) {
            auto this_ouid=unwrap(entity_ouid).value_as<std::string>();

            if(this_ouid!=ouid) {
                new_entity_ouid_list.push_back(this_ouid);
            } else {
                TRACE_(8, "StorageProvider::FileSystemDB::index::_removeConfigurableEntity() removed ouid=<" << ouid<< ">.");
            }
        }
    } catch(std::out_of_range const &) {
        TRACE_(8, "StorageProvider::FileSystemDB::index::_removeConfigurableEntity() Configuration not found, entity=<" << entity<< ">.");
    }
}
    
file_paths_t list_files_in_directory(boost::filesystem::path const& path, std::string const& ext [[gnu::unused]]) {
    auto  result = file_paths_t {};

    if(!boost::filesystem::exists(path)) {
        TRACE_(11, "StorageProvider::FileSystemDB Directory does not exist, path=<" << path.string() << ">");
        throw cet::exception("FileSystemDB") << "StorageProvider::FileSystemDB Directory does not exist, path=<" << path.string() << ">";
    }

    auto end = boost::filesystem::directory_iterator {};
    auto iter= boost::filesystem::directory_iterator(path);

    if (!boost::filesystem::exists(path) || ! boost::filesystem::is_directory(path)) {
        TRACE_(12, "StorageProvider::FileSystemDB Failed listing files in the directory; directory doesn't exist =<" << path.string() << ">");
    }

    result.reserve(100);

    for(; iter != end ; ++iter) {
        if (boost::filesystem::is_regular_file(iter->status()) && iter->path().filename().empty() ) {
            result.push_back(iter->path().filename());
        }
    }
    return result;
}

void  dbfsi::trace_enable()
{
    TRACE_CNTL( "name",    TRACE_NAME);
    TRACE_CNTL( "lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL );
    TRACE_CNTL( "modeM", 1LL );
    TRACE_CNTL( "modeS", 1LL );

    TRACE_( 0, "artdaq::database::filesystem::index" << "trace_enable");
}

using artdaq::database::sharedtypes::unwrap;
using artdaq::database::sharedtypes::unwrapper;

template<>
template<typename T>
T&  unwrapper<jsn::value_t>::value_as()
{
    return boost::get<T>(any);
}

template<>
template<typename T>
T const&  unwrapper<const jsn::value_t>::value_as()
{
    using V = typename std::remove_const<T>::type;

    return boost::get<V>(any);
}
