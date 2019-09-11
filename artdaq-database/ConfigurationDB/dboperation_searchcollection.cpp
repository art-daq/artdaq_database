#include "artdaq-database/ConfigurationDB/common.h"

#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/ConfigurationDB/dboperation_searchcollection.h"
#include "artdaq-database/ConfigurationDB/options_operations.h"
#include "artdaq-database/ConfigurationDB/shared_helper_functions.h"
#include "artdaq-database/DataFormats/shared_literals.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "dboperation_searchcollection.cpp"

using namespace artdaq::database::configuration;
namespace dbcfg = artdaq::database::configuration;

namespace artdaq {
namespace database {
namespace configuration {
namespace detail {
void search_collection(ManageDocumentOperation const&, std::string&);

}  // namespace detail
}  // namespace configuration
}  // namespace database
}  // namespace artdaq

using namespace artdaq::database::result;
using artdaq::database::result_t;

result_t opts::search_collection(ManageDocumentOperation const& options, std::string& results) noexcept {
  try {
    detail::search_collection(options, results);
    return Success(results);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}

result_t json::search_collection(std::string const& query_payload) noexcept {
  try {
    if (query_payload.empty()) {
      return Failure(msg_EmptyFilter);
    }

    auto options = ManageDocumentOperation{apiliteral::operation::listdatabases};
    options.readJsonData({query_payload});

    auto returnValue = std::string{};

    detail::search_collection(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}

void dbcfg::debug::SearchCollection() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);

  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  dbcfg::debug::detail::SearchCollection();
  TLOG(10) << "artdaq::database::configuration::SearchCollection trace_enable";
}
