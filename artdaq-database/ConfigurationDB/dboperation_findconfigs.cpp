#include "artdaq-database/ConfigurationDB/common.h"

#include <boost/exception/diagnostic_information.hpp>
#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/ConfigurationDB/dboperation_manageconfigs.h"
#include "artdaq-database/ConfigurationDB/options_operations.h"
#include "artdaq-database/ConfigurationDB/shared_helper_functions.h"
#include "artdaq-database/DataFormats/shared_literals.h"
#include "artdaq-database/SharedCommon/configuraion_api_literals.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "CONF:OpSrch_C"

using namespace artdaq::database::configuration;
namespace dbcfg = artdaq::database::configuration;

using artdaq::database::configuration::options::data_format_t;

namespace artdaq {
namespace database {
bool json_db_to_gui(std::string const&, std::string&);
bool json_gui_to_db(std::string const&, std::string&);
}
}

namespace artdaq {
namespace database {
namespace configuration {
namespace detail {
void find_configurations(ManageConfigsOperation const&, std::string&);
void configuration_composition(ManageConfigsOperation const&, std::string&);
}  // namespace detail
}  // namespace configuration
}  // namespace database
}  // namespace artdaq

using artdaq::database::result_t;

result_t opts::find_configurations(ManageConfigsOperation const& options) noexcept {
  try {
    auto returnValue = std::string{};

    detail::find_configurations(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(boost::current_exception_diagnostic_information());
  }
}

result_t opts::configuration_composition(ManageConfigsOperation const& options) noexcept {
  try {
    auto returnValue = std::string{};

    detail::configuration_composition(options, returnValue);

    return Success(returnValue);
  } catch (...) {
    return Failure(boost::current_exception_diagnostic_information());
  }
}

result_t json::find_configurations(std::string const& query_payload) noexcept {
  try {
    auto options = ManageConfigsOperation{apiliteral::operation::findconfigs};
    options.readJsonData({query_payload});

    auto returnValue = std::string{};

    detail::find_configurations(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(boost::current_exception_diagnostic_information());
  }
}

result_t json::configuration_composition(std::string const& query_payload) noexcept {
  try {
    auto options = ManageConfigsOperation{apiliteral::operation::confcomposition};
    options.readJsonData({query_payload});

    auto returnValue = std::string{};

    detail::configuration_composition(options, returnValue);

    return Success(returnValue);
  } catch (...) {
    return Failure(boost::current_exception_diagnostic_information());
  }
}

void dbcfg::debug::enableFindConfigsOperation() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);

  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TRACE_(0, "artdaq::database::configuration::FindConfigsOperation trace_enable");
}
