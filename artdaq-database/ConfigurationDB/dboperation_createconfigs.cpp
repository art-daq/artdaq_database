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
void add_configuration_to_global_configuration(LoadStoreOperation const&, std::string&);
void create_new_global_configuration(std::string const&, std::string&);
void find_configuration_versions(LoadStoreOperation const&, std::string&);
void find_configuration_entities(LoadStoreOperation const&, std::string&);
void list_collection_names(LoadStoreOperation const&, std::string&);

}  // namespace detail
}  // namespace configuration
}  // namespace database
}  // namespace artdaq

using namespace artdaq::database::result;

result_pair_t opts::add_configuration_to_global_configuration(LoadStoreOperation const& options) noexcept {
  try {
    auto returnValue = std::string{};

    detail::add_configuration_to_global_configuration(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(boost::current_exception_diagnostic_information());
  }
}

result_pair_t opts::create_new_global_configuration(LoadStoreOperation const& /*options*/) noexcept {
  return Failure("Not Implemented");
}

result_pair_t opts::list_collection_names(LoadStoreOperation const& options) noexcept {
  try {
    auto returnValue = std::string{};

    detail::list_collection_names(options, returnValue);

    return Success(returnValue);
  } catch (...) {
    return Failure(boost::current_exception_diagnostic_information());
  }
}

result_pair_t opts::find_configuration_versions(LoadStoreOperation const& options) noexcept {
  try {
    auto returnValue = std::string{};

    detail::find_configuration_versions(options, returnValue);

    return Success(returnValue);
  } catch (...) {
    return Failure(boost::current_exception_diagnostic_information());
  }
}

result_pair_t opts::find_configuration_entities(LoadStoreOperation const& options) noexcept {
  try {
    auto returnValue = std::string{};

    detail::find_configuration_entities(options, returnValue);

    return Success(returnValue);
  } catch (...) {
    return Failure(boost::current_exception_diagnostic_information());
  }
}

result_pair_t json::add_configuration_to_global_configuration(std::string const& search_filter) noexcept {
  try {
    if (search_filter.empty()) return Failure(msg_EmptyFilter);

    auto options = LoadStoreOperation{apiliteral::operation::addconfig};
    options.readJsonData({search_filter});

    auto returnValue = std::string{};

    detail::add_configuration_to_global_configuration(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(boost::current_exception_diagnostic_information());
  }
}

result_pair_t json::create_new_global_configuration(std::string const& operations) noexcept {
  try {
    if (operations.empty()) return Failure(msg_EmptyFilter);

    auto returnValue = std::string{};

    detail::create_new_global_configuration(operations, returnValue);

    return Success(returnValue);
  } catch (...) {
    return Failure(boost::current_exception_diagnostic_information());
  }
}

result_pair_t json::list_collection_names(std::string const& search_filter) noexcept {
  try {
    if (search_filter.empty()) return Failure(msg_EmptyFilter);

    auto options = LoadStoreOperation{apiliteral::operation::listcollections};
    options.readJsonData({search_filter});

    auto returnValue = std::string{};

    detail::list_collection_names(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(boost::current_exception_diagnostic_information());
  }
}

result_pair_t json::find_configuration_versions(std::string const& search_filter) noexcept {
  try {
    if (search_filter.empty()) return Failure(msg_EmptyFilter);

    auto options = LoadStoreOperation{apiliteral::operation::findversions};
    options.readJsonData({search_filter});

    auto returnValue = std::string{};

    detail::find_configuration_versions(options, returnValue);

    return Success(returnValue);
  } catch (...) {
    return Failure(boost::current_exception_diagnostic_information());
  }
}

result_pair_t json::find_configuration_entities(std::string const& search_filter) noexcept {
  try {
    if (search_filter.empty()) return Failure(msg_EmptyFilter);

    auto options = LoadStoreOperation{apiliteral::operation::findentities};
    options.readJsonData({search_filter});

    auto returnValue = std::string{};

    detail::find_configuration_entities(options, returnValue);

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
