#ifndef _ARTDAQ_DATABASE_PROVIDER_FILESYSTEM_INDEX_H_
#define _ARTDAQ_DATABASE_PROVIDER_FILESYSTEM_INDEX_H_

#include "artdaq-database/StorageProviders/common.h"
#include "artdaq-database/StorageProviders/storage_providers.h"
#include "artdaq-database/FhiclJson/json_common.h"

#include <boost/filesystem.hpp>

namespace artdaq {
namespace database {  
namespace basictypes {
class JsonData;
}

namespace filesystem { 
namespace index {

using artdaq::database::basictypes::JsonData;
using artdaq::database::object_id_t;
using artdaq::database::json::object_t;

class SearchIndex final {
public:
    SearchIndex(boost::filesystem::path const&);
    ~SearchIndex();

    std::vector<object_id_t> findDocumentIDs(JsonData const&);
    bool addDocument(JsonData const&, object_id_t const&);
    bool removeDocument(JsonData const&, object_id_t const&);
    
    //deleted
    SearchIndex() = delete;
    SearchIndex& operator= ( SearchIndex const& ) = delete;
    SearchIndex& operator= ( SearchIndex && ) = delete;
    SearchIndex ( SearchIndex && ) = delete;

private:
    void  _addVersion(object_id_t const&,std::string const&);
    void  _addConfiguration(object_id_t const&,std::string const&);
    void  _removeVersion(object_id_t const&,std::string const&);
    void  _removeConfiguration(object_id_t const&,std::string const&);
    std::vector<object_id_t>  _matchVersion(std::string const&);
    std::vector<object_id_t>  _matchConfiguration(std::string const&);
    std::vector<object_id_t>  _matchObjectId(std::string const&);
    std::vector<object_id_t>  _matchObjectIds(std::string const&); 

private:
    bool _open(boost::filesystem::path const&);
    bool _create(boost::filesystem::path const&);    
    bool _close();
private:
    object_t  _index;
    boost::filesystem::path _path;
    bool _isOpen;
};

void  trace_enable();

} //namespace index
} //namespace filesystem
} //namespace database
} //namespace artdaq

#endif /* _ARTDAQ_DATABASE_PROVIDER_FILESYSTEM_INDEX_H_ */
