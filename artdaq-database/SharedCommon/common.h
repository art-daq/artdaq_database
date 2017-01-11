#ifndef _ARTDAQ_DATABASE_COMMON_H_
#define _ARTDAQ_DATABASE_COMMON_H_

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <memory>
#include <regex>
#include <sstream>
#include <streambuf>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>
#include <iomanip>

#include <boost/core/demangle.hpp>
#include <boost/lexical_cast.hpp>

#include "trace.h"

# define confirm(expr) assert(expr)
#endif /* _ARTDAQ_DATABASE_COMMON_H_ */
