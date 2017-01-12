#include "artdaq-database/ConfigurationDB/common.h"

#include <boost/exception/diagnostic_information.hpp>
#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/ConfigurationDB/dboperation_managealiases.h"
#include "artdaq-database/ConfigurationDB/options_operations.h"
#include "artdaq-database/ConfigurationDB/shared_helper_functions.h"
#include "artdaq-database/DataFormats/shared_literals.h"
#include "artdaq-database/SharedCommon/configuraion_api_literals.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "CONF:OpVerAls_C"

using namespace artdaq::database::configuration;
namespace dbcfg = artdaq::database::configuration;

namespace jsonliteral = artdaq::database::dataformats::literal;
namespace apiliteral = artdaq::database::configapi::literal;

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
// reserved

}  // namespace detail
}  // namespace configuration
}  // namespace database
}  // namespace artdaq

using namespace artdaq::database::result;

result_pair_t json::add_version_alias(std::string const& search_filter) noexcept {
  try {
    if (search_filter.empty()) return Failure(msg_EmptyFilter);

    auto options = LoadStoreOperation{apiliteral::operation::addalias};
    options.readJsonData({search_filter});

    auto returnValue = std::string{};

    // detail::add_version_alias(options, returnValue);

    return Success(returnValue);
  } catch (...) {
    return Failure(boost::current_exception_diagnostic_information());
  }
}

result_pair_t json::remove_version_alias(std::string const& search_filter) noexcept {
  try {
    if (search_filter.empty()) return Failure(msg_EmptyFilter);

    auto options = LoadStoreOperation{apiliteral::operation::rmalias};
    options.readJsonData({search_filter});

    auto returnValue = std::string{};

    // detail::remove_version_alias(options, returnValue);

    return Success(returnValue);
  } catch (...) {
    return Failure(boost::current_exception_diagnostic_information());
  }
}

result_pair_t json::find_version_aliases(std::string const& search_filter) noexcept {
  try {
    if (search_filter.empty()) return Failure(msg_EmptyFilter);

    auto options = LoadStoreOperation{apiliteral::operation::rmalias};
    options.readJsonData({search_filter});

    auto returnValue = std::string{};

    // detail::find_version_aliases(options, returnValue);

    return Success(returnValue);
  } catch (...) {
    return Failure(boost::current_exception_diagnostic_information());
  }
}

void dbcfg::debug::enableVersionAliasOperation() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);

  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TRACE_(0, "artdaq::database::configuration::CreateConfigsOperation trace_enable");
}
