#include "artdaq-database/ConfigurationDB/common.h"

#include "artdaq-database/ConfigurationDB/dboperation_findcompositions.h"
#include "artdaq-database/ConfigurationDB/dispatch_filedb.h"
#include "artdaq-database/ConfigurationDB/dispatch_mongodb.h"
#include "artdaq-database/ConfigurationDB/dispatch_ucondb.h"
#include "artdaq-database/ConfigurationDB/options_operations.h"

#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/DataFormats/shared_literals.h"
#include "artdaq-database/JsonDocument/JSONDocument.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/SharedCommon/helper_functions.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "detail_findcompositions.cpp"

using namespace artdaq::database;
using namespace artdaq::database::configuration;

namespace cf = artdaq::database::configuration;
namespace db = artdaq::database;
namespace jsn = artdaq::database::json;

using Options = cf::ManageDocumentOperation;
using artdaq::database::docrecord::JSONDocument;

namespace apiliteral = artdaq::database::configapi::literal;
namespace jsonliteral = artdaq::database::dataformats::literal;

using provider_call_returnslist_t = std::vector<JSONDocument> (*)(Options const&, JSONDocument const&);

namespace {
void validate_dbprovider_name(std::string const& name) {
  auto providers = std::list<std::string>{apiliteral::provider::mongo, apiliteral::provider::filesystem, apiliteral::provider::ucon};

  if (std::find(providers.begin(), providers.end(), name) == providers.end()) {
    throw runtime_error("validate_dbprovider_name") << "Unknown provider name=<" << name << ">.";
  }
}

}  // namespace

namespace artdaq {
namespace database {
namespace configuration {
namespace detail {

void find_compositions_containing(Options const& options, std::string& results) {
  confirm(results.empty());
  confirm(options.operation() == apiliteral::operation::findcompositionscontaining);

  TLOG(21) << "find_compositions_containing: begin";
  TLOG(21) << "find_compositions_containing args options=<" << options << ">";

  validate_dbprovider_name(options.provider());

  auto dispatch_persistence_provider = [](std::string const& name) -> provider_call_returnslist_t {
    auto providers =
        std::map<std::string, provider_call_returnslist_t>{{apiliteral::provider::mongo, cf::mongo::findCompositionsContaining},
                                                           {apiliteral::provider::filesystem, cf::filesystem::findCompositionsContaining},
                                                           {apiliteral::provider::ucon, cf::ucon::findCompositionsContaining}};

    return providers.at(name);
  };

  auto search_results = dispatch_persistence_provider(options.provider())(options, options.query_filter_to_JsonData());

  auto returnValue = std::string{};
  auto returnValueChanged = bool{false};

  switch (options.format()) {
    default:
    case data_format_t::db:
    case data_format_t::json:
    case data_format_t::unknown:
    case data_format_t::fhicl:
    case data_format_t::xml: {
      throw runtime_error("find_compositions_containing") << "Unsupported data format.";
      break;
    }

    case data_format_t::gui: {
      std::ostringstream oss;
      oss << "{ \"search\": [\n";

      bool first = true;
      for (auto const& search_result : search_results) {
        if (!first) oss << ",\n";
        oss << search_result;
        first = false;
      }

      oss << "\n] }";
      returnValue = oss.str();
      returnValueChanged = true;
      break;
    }

    case data_format_t::csv: {
      std::ostringstream oss;
      for (auto const& search_result : search_results) {
        try {
          auto name_node = search_result.findChild(apiliteral::name);
          oss << JSONDocument::value(name_node.value()) << ",";
        } catch (std::exception const&) {
          // Skip entries without name field
        }
      }

      if (oss.tellp() > 0) {
        oss.seekp(-1, oss.cur);  // Remove trailing comma
      }

      returnValue = oss.str();
      returnValueChanged = true;
      break;
    }
  }

  if (!returnValueChanged) {
    throw runtime_error("find_compositions_containing") << "Results were not set.";
  }

  results.swap(returnValue);

  TLOG(21) << "find_compositions_containing: end";
}

}  // namespace detail
}  // namespace configuration
}  // namespace database
}  // namespace artdaq
