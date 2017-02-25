#ifndef _ARTDAQ_DATABASE_TEST_COMMON_H_
#define _ARTDAQ_DATABASE_TEST_COMMON_H_

#include "artdaq-database/SharedCommon/sharedcommon_common.h"
#include "artdaq-database/DataFormats/shared_literals.h"
#include "artdaq-database/SharedCommon/configuraion_api_literals.h"
#include "artdaq-database/SharedCommon/helper_functions.h"

#include "boost/program_options.hpp"

constexpr auto tmpdir = "/tmp/artdaqtest";

namespace db = artdaq::database;
namespace jsonliteral = db::dataformats::literal;
namespace apiliteral = db::configapi::literal;

#endif /* _ARTDAQ_DATABASE_TEST_COMMON_H_ */
