#include "artdaq-database/ConfigurationDB/common.h"

#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/ConfigurationDB/dboperation_metadata.h"
#include "artdaq-database/ConfigurationDB/options_operations.h"
#include "artdaq-database/ConfigurationDB/shared_helper_functions.h"
#include "artdaq-database/DataFormats/shared_literals.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "dboperation_metadata.cpp"

using namespace artdaq::database::configuration;
namespace dbcfg = artdaq::database::configuration;

namespace artdaq {
namespace database {
namespace configuration {
namespace detail {
void list_databases(ManageDocumentOperation const&, std::string&);
void read_dbinfo(ManageDocumentOperation const&, std::string&);
void list_collections(ManageDocumentOperation const&, std::string&);

}  // namespace detail
}  // namespace configuration
}  // namespace database
}  // namespace artdaq

using namespace artdaq::database::result;
using artdaq::database::result_t;

result_t opts::list_databases(ManageDocumentOperation const& options, std::string& conf) noexcept {
  try {
    detail::list_databases(options, conf);
    return Success(conf);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}

result_t opts::read_dbinfo(ManageDocumentOperation const& options, std::string& conf) noexcept {
  try {
    detail::read_dbinfo(options, conf);
    return Success(conf);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}

result_t opts::list_collections(ManageDocumentOperation const& options) noexcept {
  try {
    auto returnValue = std::string{};

    detail::list_collections(options, returnValue);

    return Success(returnValue);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}

result_t json::list_databases(std::string const& query_payload) noexcept {
  try {
    if (query_payload.empty()) {
      return Failure(msg_EmptyFilter);
    }

    auto options = ManageDocumentOperation{apiliteral::operation::listdatabases};
    options.readJsonData({query_payload});

    auto returnValue = std::string{};

    detail::list_databases(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}

result_t json::read_dbinfo(std::string const& query_payload) noexcept {
  try {
    if (query_payload.empty()) {
      return Failure(msg_EmptyFilter);
    }

    auto options = ManageDocumentOperation{apiliteral::operation::readdbinfo};
    options.readJsonData({query_payload});

    auto returnValue = std::string{};

    detail::read_dbinfo(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}

result_t json::list_collections(std::string const& query_payload) noexcept {
  try {
    if (query_payload.empty()) {
      return Failure(msg_EmptyFilter);
    }

    auto options = ManageDocumentOperation{apiliteral::operation::listcollections};
    options.readJsonData({query_payload});

    auto returnValue = std::string{};

    detail::list_collections(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}
void dbcfg::debug::Metadata() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);

  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  dbcfg::debug::detail::Metadata();
  TLOG(10) << "artdaq::database::configuration::Metadata trace_enable";
}
