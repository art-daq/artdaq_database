#include "artdaq-database/ConfigurationDB/common.h"

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
void find_configurations(ManageDocumentOperation const&, std::string&);
void configuration_composition(ManageDocumentOperation const&, std::string&);
void assign_configuration(ManageDocumentOperation const&, std::string&);
void remove_configuration(ManageDocumentOperation const&, std::string&);
void create_configuration(std::string const&, std::string&);

}  // namespace detail
}  // namespace configuration
}  // namespace database
}  // namespace artdaq

using artdaq::database::result_t;

result_t opts::find_configurations(ManageDocumentOperation const& options) noexcept {
  try {
    auto returnValue = std::string{};

    detail::find_configurations(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}

result_t opts::configuration_composition(ManageDocumentOperation const& options) noexcept {
  try {
    auto returnValue = std::string{};

    detail::configuration_composition(options, returnValue);

    return Success(returnValue);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}

result_t opts::assign_configuration(ManageDocumentOperation const& options) noexcept {
  try {
    auto returnValue = std::string{};

    detail::assign_configuration(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}

result_t opts::remove_configuration(ManageDocumentOperation const& options) noexcept {
  try {
    auto returnValue = std::string{};

    detail::remove_configuration(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}

result_t opts::create_configuration(ManageDocumentOperation const& /*options*/) noexcept {
  return Failure("Not Implemented");
}

result_t json::find_configurations(std::string const& query_payload) noexcept {
  try {
    auto options = ManageDocumentOperation{apiliteral::operation::findconfigs};
    options.readJsonData({query_payload});

    auto returnValue = std::string{};

    detail::find_configurations(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}

result_t json::configuration_composition(std::string const& query_payload) noexcept {
  try {
    auto options = ManageDocumentOperation{apiliteral::operation::confcomposition};
    options.readJsonData({query_payload});

    auto returnValue = std::string{};

    detail::configuration_composition(options, returnValue);

    return Success(returnValue);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}

using namespace artdaq::database::result;

result_t json::assign_configuration(std::string const& query_payload) noexcept {
  try {
    if (query_payload.empty()) return Failure(msg_EmptyFilter);

    auto options = ManageDocumentOperation{apiliteral::operation::assignconfig};
    options.readJsonData({query_payload});

    auto returnValue = std::string{};

    detail::assign_configuration(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}

result_t json::remove_configuration(std::string const& query_payload) noexcept {
  try {
    if (query_payload.empty()) return Failure(msg_EmptyFilter);

    auto options = ManageDocumentOperation{apiliteral::operation::removeconfig};
    options.readJsonData({query_payload});

    auto returnValue = std::string{};

    detail::remove_configuration(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}

result_t json::create_configuration(std::string const& operations) noexcept {
  try {
    if (operations.empty()) return Failure(msg_EmptyFilter);

    auto returnValue = std::string{};

    detail::create_configuration(operations, returnValue);

    return Success(returnValue);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}

void dbcfg::debug::ManageConfigs() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);

  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  dbcfg::debug::detail::ManageConfigs();
  TRACE_(0, "artdaq::database::configuration::ManageConfigs trace_enable");
}
