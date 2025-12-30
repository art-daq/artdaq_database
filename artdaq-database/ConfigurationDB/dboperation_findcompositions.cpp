#include "artdaq-database/ConfigurationDB/common.h"

#include "artdaq-database/ConfigurationDB/dboperation_findcompositions.h"
#include "artdaq-database/ConfigurationDB/options_operations.h"

#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/DataFormats/shared_literals.h"
#include "artdaq-database/JsonDocument/JSONDocument.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "dboperation_findcompositions.cpp"

using namespace artdaq::database;
using namespace artdaq::database::configuration;

namespace cf = artdaq::database::configuration;
namespace db = artdaq::database;

namespace apiliteral = artdaq::database::configapi::literal;
namespace jsonliteral = artdaq::database::dataformats::literal;

namespace artdaq {
namespace database {
namespace configuration {
namespace detail {
void find_compositions_containing(ManageDocumentOperation const&, std::string&);
}  // namespace detail
}  // namespace configuration
}  // namespace database
}  // namespace artdaq

using artdaq::database::result_t;

result_t opts::find_compositions_containing(ManageDocumentOperation const& options, std::string& results) noexcept {
  try {
    auto returnValue = std::string{};

    detail::find_compositions_containing(options, returnValue);

    results.swap(returnValue);

    return Success(results);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}

result_t cf::json::find_compositions_containing(std::string const& task_payload) noexcept {
  try {
    confirm(!task_payload.empty());

    auto search_payload = basictypes::JsonData{task_payload};

    auto options_reader = ManageDocumentOperation{search_payload};

    auto returnValue = std::string{};

    auto result = opts::find_compositions_containing(options_reader, returnValue);

    if (result.first) {
      return Success(returnValue);
    }

    return result;
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}

void cf::debug::FindCompositions() { TLOG(10) << "artdaq::database::configuration::debug::FindCompositions trace_enable"; }

void cf::debug::detail::FindCompositions() { TLOG(11) << "artdaq::database::configuration::debug::detail::FindCompositions trace_enable"; }
