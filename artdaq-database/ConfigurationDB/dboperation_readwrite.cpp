#include "artdaq-database/ConfigurationDB/common.h"

#include <boost/exception/diagnostic_information.hpp>
#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/ConfigurationDB/dboperation_readwrite.h"
#include "artdaq-database/ConfigurationDB/options_operations.h"
#include "artdaq-database/ConfigurationDB/shared_helper_functions.h"
#include "artdaq-database/DataFormats/shared_literals.h"
#include "artdaq-database/SharedCommon/configuraion_api_literals.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "CONF:OpldStr_C"

using namespace artdaq::database::configuration;
namespace dbcfg = artdaq::database::configuration;

using artdaq::database::configuration::options::data_format_t;

namespace jsonliteral = artdaq::database::dataformats::literal;
namespace apiliteral = artdaq::database::configapi::literal;

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
void write_document(ManageDocumentOperation const&, std::string&);
void read_document(ManageDocumentOperation const&, std::string&);
}  // namespace detail
}  // namespace configuration
}  // namespace database
}  // namespace artdaq

using namespace artdaq::database::result;
using artdaq::database::result_t;

result_t opts::write_document(ManageDocumentOperation const& options, std::string& conf) noexcept {
  try {
    detail::write_document(options, conf);
    return Success(conf);
  } catch (...) {
    return Failure(boost::current_exception_diagnostic_information());
  }
}

result_t opts::read_document(ManageDocumentOperation const& options, std::string& conf) noexcept {
  try {
    detail::read_document(options, conf);
    return Success(conf);
  } catch (...) {
    return Failure(boost::current_exception_diagnostic_information());
  }
}

result_t json::write_document(std::string const& query_payload, std::string const& conf) noexcept {
  try {
    if (query_payload.empty()) return Failure(msg_EmptyFilter);
    if (conf.empty()) return Failure(msg_EmptyDocument);

    auto options = ManageDocumentOperation{apiliteral::operation::writedocument};
    options.readJsonData({query_payload});

    // convert to database_format
    auto database_format = std::string(conf);

    detail::write_document(options, database_format);
    return Success(database_format);
  } catch (...) {
    return Failure(boost::current_exception_diagnostic_information());
  }
}

result_t json::read_document(std::string const& query_payload, std::string& conf) noexcept {
  try {
    if (query_payload.empty()) return Failure(msg_EmptyFilter);

    auto options = ManageDocumentOperation{apiliteral::operation::readdocument};
    options.readJsonData({query_payload});

    auto database_format = std::string{};

    detail::read_document(options, database_format);

    // convert to gui
    conf = database_format;

    return Success(conf);
  } catch (...) {
    return Failure(boost::current_exception_diagnostic_information());
  }
}

void dbcfg::debug::enableManageDocumentOperation() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);

  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TRACE_(0, "artdaq::database::configuration::ManageDocumentOperation trace_enable");
}