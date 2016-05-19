#ifndef _ARTDAQ_DATABASE_TEST_COMMON_H_
#define _ARTDAQ_DATABASE_TEST_COMMON_H_

#include <cassert>
#include <ctime>
#include <iostream>
#include <memory>
#include <tuple>
#include <type_traits>
#include <vector>


#include <fstream>
#include <iostream>
#include <streambuf>
#include <string>
#include  <cstdio>
#include <libgen.h>
#include "artdaq-database/BuildInfo/printStackTrace.h"
#include "artdaq-database/BuildInfo/process_exit_codes.h"
#include "boost/program_options.hpp"
#include <boost/exception/diagnostic_information.hpp>
#include <boost/core/demangle.hpp>
#include "trace.h"

constexpr auto tmpdir = "/tmp/artdaqtest/";

namespace artdaq {
namespace database {
std::string mkdir(std::string const&);
}}
#endif /* _ARTDAQ_DATABASE_TEST_COMMON_H_ */
