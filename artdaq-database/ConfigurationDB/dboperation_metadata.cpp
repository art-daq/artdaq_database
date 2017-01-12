#include "artdaq-database/ConfigurationDB/common.h"

#include <boost/exception/diagnostic_information.hpp>
#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/ConfigurationDB/dboperation_metadata.h"
#include "artdaq-database/ConfigurationDB/options_operations.h"
#include "artdaq-database/ConfigurationDB/shared_helper_functions.h"
#include "artdaq-database/ConfigurationDB/shared_helper_functions.h"
#include "artdaq-database/DataFormats/shared_literals.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "CONF:OpMDA_C"

using namespace artdaq::database::configuration;
namespace dbcfg = artdaq::database::configuration;

using artdaq::database::configuration::options::data_format_t;

namespace artdaq {
namespace database {
namespace configuration {
namespace detail {
void list_database_names(LoadStoreOperation const&, std::string&);
void read_database_info(LoadStoreOperation const&, std::string&);

}  // namespace detail
}  // namespace configuration
}  // namespace database
}  // namespace artdaq

using namespace artdaq::database::result;

result_pair_t opts::list_database_names(LoadStoreOperation const& options, std::string& conf) noexcept {
  try {
    detail::list_database_names(options, conf);
    return Success(conf);
  } catch (...) {
    return Failure(boost::current_exception_diagnostic_information());
  }
}

result_pair_t opts::read_database_info(LoadStoreOperation const& options, std::string& conf) noexcept {
  try {
    detail::read_database_info(options, conf);
    return Success(conf);
  } catch (...) {
    return Failure(boost::current_exception_diagnostic_information());
  }
}

result_pair_t json::list_database_names(std::string const& search_filter) noexcept {
  try {
    if (search_filter.empty()) return Failure(msg_EmptyFilter);

    auto options = LoadStoreOperation{apiliteral::operation::listdatabases};
    options.readJsonData({search_filter});

    auto returnValue = std::string{};

    detail::list_database_names(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(boost::current_exception_diagnostic_information());
  }
}

result_pair_t json::read_database_info(std::string const& search_filter) noexcept {
  try {
    if (search_filter.empty()) return Failure(msg_EmptyFilter);

    auto options = LoadStoreOperation{apiliteral::operation::readdbinfo};
    options.readJsonData({search_filter});

    auto returnValue = std::string{};

    detail::read_database_info(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(boost::current_exception_diagnostic_information());
  }
}

void dbcfg::debug::enableMetadataOperation() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);

  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TRACE_(0, "artdaq::database::configuration::MetadataOperation trace_enable");
}
