#include "artdaq-database/ConfigurationDB/common.h"

#include <boost/exception/diagnostic_information.hpp>
#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/BuildInfo/process_exit_codes.h"
#include "artdaq-database/ConfigurationDB/dboperation_loadstore.h"
#include "artdaq-database/ConfigurationDB/options_operations.h"
#include "artdaq-database/ConfigurationDB/shared_helper_functions.h"
#include "artdaq-database/ConfigurationDB/shared_literals.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "CONF:OpldStr_C"

using namespace artdaq::database::configuration;

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
void store_configuration(LoadStoreOperation const&, std::string&);
void load_configuration(LoadStoreOperation const&, std::string&);
}  // namespace detail
}  // namespace configuration
}  // namespace database
}  // namespace artdaq

auto make_error_msg = [](auto msg) { return std::string("{error:\"").append(msg).append(".\"}"); };

result_pair_t opts::store_configuration(LoadStoreOperation const& options, std::string& conf) noexcept {
  try {
    detail::store_configuration(options, conf);
    return result_pair_t{true, literal::result::SUCCESS};
  } catch (...) {
    return result_pair_t{false, boost::current_exception_diagnostic_information()};
  }
}

result_pair_t opts::load_configuration(LoadStoreOperation const& options, std::string& conf) noexcept {
  try {
    detail::load_configuration(options, conf);
    return result_pair_t{true, literal::result::SUCCESS};
  } catch (...) {
    return result_pair_t{false, boost::current_exception_diagnostic_information()};
  }
}

result_pair_t json::store_configuration(std::string const& search_filter, std::string const& conf) noexcept {
  try {
    if (search_filter.empty()) return std::make_pair(false, make_error_msg(literal::msg::empty_filter));
    if (conf.empty()) return std::make_pair(false, make_error_msg(literal::msg::empty_document));

    auto options = LoadStoreOperation{literal::operation::store};
    options.readJsonData({search_filter});

    // convert to database_format
    auto database_format = std::string(conf);

    detail::store_configuration(options, database_format);
    return result_pair_t{true, database_format};
  } catch (...) {
    return result_pair_t{false, boost::current_exception_diagnostic_information()};
  }
}

result_pair_t json::load_configuration(std::string const& search_filter, std::string& conf) noexcept {
  try {
    if (search_filter.empty()) return std::make_pair(false, make_error_msg(literal::msg::empty_filter));

    auto options = LoadStoreOperation{literal::operation::load};
    options.readJsonData({search_filter});

    auto database_format = std::string(conf);

    detail::load_configuration(options, database_format);

    // convert to gui
    conf = database_format;

    return result_pair_t{true, literal::result::SUCCESS};
  } catch (...) {
    return result_pair_t{false, boost::current_exception_diagnostic_information()};
  }
}

void debug::enableLoadStoreOperation() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);

  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TRACE_(0, "artdaq::database::configuration::LoadStoreOperation trace_enable");
}