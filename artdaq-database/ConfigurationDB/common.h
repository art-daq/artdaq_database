#ifndef _ARTDAQ_DATABASE_CONFIGURATIONDB_COMMON_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONDB_COMMON_H_

#include <algorithm>
#include <cassert>
#include <ctime>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

#include "cetlib/coded_exception.h"
#include "trace.h"

#include <boost/core/demangle.hpp>

namespace artdaq {
namespace database {
namespace configuration {

using result_pair_t = std::pair<bool, std::string>;

namespace options {
class JsonSerializable;

}  // namespace options
}  // namespace configuration
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_CONFIGURATIONDB_COMMON_H_ */
