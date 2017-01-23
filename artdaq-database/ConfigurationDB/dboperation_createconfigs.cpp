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

#define TRACE_NAME "CONF:OpCrtCfs_C"

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
void assign_configuration(ManageDocumentOperation const&, std::string&);
void remove_configuration(ManageDocumentOperation const&, std::string&);
void create_configuration(std::string const&, std::string&);
void find_versions(ManageDocumentOperation const&, std::string&);
void find_entities(ManageDocumentOperation const&, std::string&);
void list_collections(ManageDocumentOperation const&, std::string&);

}  // namespace detail
}  // namespace configuration
}  // namespace database
}  // namespace artdaq

using namespace artdaq::database::result;
using artdaq::database::result_t;

result_t opts::assign_configuration(ManageDocumentOperation const& options) noexcept {
  try {
    auto returnValue = std::string{};

    detail::assign_configuration(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(boost::current_exception_diagnostic_information());
  }
}

result_t opts::remove_configuration(ManageDocumentOperation const& options) noexcept {
  try {
    auto returnValue = std::string{};

    detail::remove_configuration(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(boost::current_exception_diagnostic_information());
  }
}

result_t opts::create_configuration(ManageDocumentOperation const& /*options*/) noexcept {
  return Failure("Not Implemented");
}

result_t opts::list_collections(ManageDocumentOperation const& options) noexcept {
  try {
    auto returnValue = std::string{};

    detail::list_collections(options, returnValue);

    return Success(returnValue);
  } catch (...) {
    return Failure(boost::current_exception_diagnostic_information());
  }
}

result_t opts::find_versions(ManageDocumentOperation const& options) noexcept {
  try {
    auto returnValue = std::string{};

    detail::find_versions(options, returnValue);

    return Success(returnValue);
  } catch (...) {
    return Failure(boost::current_exception_diagnostic_information());
  }
}

result_t opts::find_entities(ManageDocumentOperation const& options) noexcept {
  try {
    auto returnValue = std::string{};

    detail::find_entities(options, returnValue);

    return Success(returnValue);
  } catch (...) {
    return Failure(boost::current_exception_diagnostic_information());
  }
}

result_t json::assign_configuration(std::string const& query_payload) noexcept {
  try {
    if (query_payload.empty()) return Failure(msg_EmptyFilter);

    auto options = ManageDocumentOperation{apiliteral::operation::assignconfig};
    options.readJsonData({query_payload});

    auto returnValue = std::string{};

    detail::assign_configuration(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(boost::current_exception_diagnostic_information());
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
    return Failure(boost::current_exception_diagnostic_information());
  }
}

result_t json::create_configuration(std::string const& operations) noexcept {
  try {
    if (operations.empty()) return Failure(msg_EmptyFilter);

    auto returnValue = std::string{};

    detail::create_configuration(operations, returnValue);

    return Success(returnValue);
  } catch (...) {
    return Failure(boost::current_exception_diagnostic_information());
  }
}

result_t json::list_collections(std::string const& query_payload) noexcept {
  try {
    if (query_payload.empty()) return Failure(msg_EmptyFilter);

    auto options = ManageDocumentOperation{apiliteral::operation::listcollections};
    options.readJsonData({query_payload});

    auto returnValue = std::string{};

    detail::list_collections(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(boost::current_exception_diagnostic_information());
  }
}

result_t json::find_versions(std::string const& query_payload) noexcept {
  try {
    if (query_payload.empty()) return Failure(msg_EmptyFilter);

    auto options = ManageDocumentOperation{apiliteral::operation::findversions};
    options.readJsonData({query_payload});

    auto returnValue = std::string{};

    detail::find_versions(options, returnValue);

    return Success(returnValue);
  } catch (...) {
    return Failure(boost::current_exception_diagnostic_information());
  }
}

result_t json::find_entities(std::string const& query_payload) noexcept {
  try {
    if (query_payload.empty()) return Failure(msg_EmptyFilter);

    auto options = ManageDocumentOperation{apiliteral::operation::findentities};
    options.readJsonData({query_payload});

    auto returnValue = std::string{};

    detail::find_entities(options, returnValue);

    return Success(returnValue);
  } catch (...) {
    return Failure(boost::current_exception_diagnostic_information());
  }
}

void dbcfg::debug::enableCreateConfigsOperation() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);

  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TRACE_(0, "artdaq::database::configuration::CreateConfigsOperation trace_enable");
}
