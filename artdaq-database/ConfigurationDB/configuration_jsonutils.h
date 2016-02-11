#ifndef _ARTDAQ_DATABASE_CONFIGURATION_JSONUTILS_H_
#define _ARTDAQ_DATABASE_CONFIGURATION_JSONUTILS_H_

#include "artdaq-database/ConfigurationDB/common.h"
#include "artdaq-database/BasicTypes/basictypes.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "ConfigJsonUtils_H"


namespace artdaq{
namespace database{
namespace configuration{
namespace jsonutils{
using artdaq::database::basictypes::JsonData;
  
class JSONDocument final
{
   void insertNode(std::string const &, JsonData const&);
   void replaceNode(std::string const &, JsonData const&);
   void deleteNode(std::string const &);
   void extractNode(std::string const &, JsonData&);

private:
   std::vector<std::string> split_path(std::string const&);
   
private:
  std::string json_buffer;
};
  

} //namespace jsonutils
} //namespace configuration
} //namespace database
} //namespace artdaq
#endif /* _ARTDAQ_DATABASE_CONFIGURATION_JSONUTILS_H_ */
