#include "artdaq-database/ConfigurationDB/common.h"

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
void add_version_alias(ManageAliasesOperation const& options, std::string& conf);
void remove_version_alias(ManageAliasesOperation const& options, std::string& conf);
void find_version_aliases(ManageAliasesOperation const& options, std::string& conf);
}  // namespace detail
}  // namespace configuration
}  // namespace database
}  // namespace artdaq

using namespace artdaq::database::result;

result_t json::add_version_alias(std::string const& query_payload) noexcept {
  try {
    if (query_payload.empty()) return Failure(msg_EmptyFilter);

    auto options = ManageAliasesOperation{apiliteral::operation::addversionalias};
    options.readJsonData({query_payload});

    auto returnValue = std::string{};

    detail::add_version_alias(options, returnValue);

    return Success(returnValue);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}

result_t json::remove_version_alias(std::string const& query_payload) noexcept {
  try {
    if (query_payload.empty()) return Failure(msg_EmptyFilter);

    auto options = ManageAliasesOperation{apiliteral::operation::rmversionalias};
    options.readJsonData({query_payload});

    auto returnValue = std::string{};

    detail::remove_version_alias(options, returnValue);

    return Success(returnValue);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}

result_t json::find_version_aliases(std::string const& query_payload) noexcept {
  try {
    if (query_payload.empty()) return Failure(msg_EmptyFilter);

    auto options = ManageAliasesOperation{apiliteral::operation::rmversionalias};
    options.readJsonData({query_payload});

    auto returnValue = std::string{};

    detail::find_version_aliases(options, returnValue);

    return Success(returnValue);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}

void dbcfg::debug::ManageAliases() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);

  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  dbcfg::debug::detail::ManageAliases();
  TRACE_(0, "artdaq::database::configuration::ManageAliases trace_enable");
}
