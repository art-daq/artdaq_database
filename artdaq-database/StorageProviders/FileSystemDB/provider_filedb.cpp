#include "artdaq-database/StorageProviders/common.h"
#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h"

#include "artdaq-database/BasicTypes/basictypes.h"



namespace artdaq{
namespace database{

using artdaq::database::basictypes::JsonData;
using artdaq::database::filesystem::FileSystemDB;
  
template<>
template <>
std::vector<JsonData> StorageProvider<JsonData, FileSystemDB>::load(JsonData const& filter [[gnu::unused]])
{
    //FIXME:: implement this
    auto returnCollection = std::vector<JsonData>();

    return returnCollection;
}


template<>
void StorageProvider<JsonData, FileSystemDB>::store(JsonData const& data [[gnu::unused]])
{
  //FIXME:: implement this
}

} //namespace database
} //namespace artdaq
