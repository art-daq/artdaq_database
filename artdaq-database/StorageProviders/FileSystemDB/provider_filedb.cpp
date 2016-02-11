#include "artdaq-database/StorageProviders/common.h"
#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h"

#include "artdaq-database/BasicTypes/basictypes.h"

#ifdef TRACE_NAME
  #undef TRACE_NAME
#endif

#define TRACE_NAME "PRVDR:FileDB_C"

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
object_id_t StorageProvider<JsonData, FileSystemDB>::store(JsonData const& data [[gnu::unused]])
{
  //FIXME:: implement this
  return "error";
}

namespace filesystem{
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
