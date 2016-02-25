#ifndef _ARTDAQ_DATABASE_CONFIGURATIONDB_COMMON_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONDB_COMMON_H_

#include <memory>
#include <tuple>
#include <vector>
#include <ctime>
#include <cassert>
#include <type_traits>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <map>

#include "trace.h"
#include "cetlib/coded_exception.h"

#include <boost/core/demangle.hpp>
  


namespace artdaq{
namespace database{
namespace configuration{

using result_pair_t=std::pair<bool, std::string>;  

namespace options{    
class JsonSerializable {
//palceholder
};

} //namespace options
} //namespace configuration
} //namespace database
} //namespace artdaq


#endif /* _ARTDAQ_DATABASE_CONFIGURATIONDB_COMMON_H_ */
