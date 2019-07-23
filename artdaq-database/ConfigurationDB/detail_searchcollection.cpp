#include "artdaq-database/ConfigurationDB/common.h"

#include "artdaq-database/ConfigurationDB/dboperation_searchcollection.h"

#include "artdaq-database/ConfigurationDB/shared_helper_functions.h"
#include "artdaq-database/DataFormats/shared_literals.h"
#include "artdaq-database/SharedCommon/configuraion_api_literals.h"

#include "artdaq-database/ConfigurationDB/configuration_dbproviders.h"

#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/DataFormats/Json/json_common.h"

#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/JsonDocument/docrecord_literals.h"

#include "artdaq-database/ConfigurationDB/options_operations.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "detail_searchcollection.cpp"

namespace db = artdaq::database;
namespace cf = db::configuration;
namespace cftd = cf::debug::detail;

using cf::options::data_format_t;

using Options = cf::ManageDocumentOperation;

using artdaq::database::docrecord::JSONDocument;

namespace artdaq {
namespace database {
bool json_db_to_gui(std::string const&, std::string&);
bool json_gui_to_db(std::string const&, std::string&);
}  // namespace database
}  // namespace artdaq

namespace artdaq {
namespace database {
namespace configuration {
namespace detail {

using provider_searchcollection_t = std::vector<JSONDocument> (*)(const Options&, const JSONDocument&);

void search_collection(Options const& options, std::string& collections) {
  confirm(collections.empty());
  confirm(options.operation() == apiliteral::operation::listcollections);

  TLOG(23) << "search_collection: begin";
  TLOG(23) << "search_collection args query_payload=<" << options << ">";

  validate_dbprovider_name(options.provider());

  auto dispatch_persistence_provider = [](std::string const& name) {
    auto providers = std::map<std::string, provider_searchcollection_t>{{apiliteral::provider::mongo, cf::mongo::listCollections},
                                                                       {apiliteral::provider::filesystem, cf::filesystem::listCollections},
                                                                       {apiliteral::provider::ucon, cf::ucon::listCollections}};

    return providers.at(name);
  };

  auto search_results = dispatch_persistence_provider(options.provider())(options, options.collection_to_JsonData());

  auto returnValue = std::string{};
  auto returnValueChanged = bool{false};

  switch (options.format()) {
    default:
    case data_format_t::db:
    case data_format_t::gui:
    case data_format_t::unknown:
    case data_format_t::fhicl:
		case data_format_t::csv:
    case data_format_t::xml: {
      throw runtime_error("search_collection") << "Unsupported data format.";
      break;
    }

    case data_format_t::json: {
      std::ostringstream oss;
      oss << "{ \"search\": [\n";
      for (auto const& search_result : search_results) {
        oss << search_result << ",";
      }

      oss.seekp(-1, oss.cur);
      oss << "] }";
      returnValue = oss.str();
      returnValueChanged = true;
      break;
    }
  }

  if (returnValueChanged) {
    collections.swap(returnValue);
  }

  TLOG(23) << "search_collection: end";
}

}  // namespace detail
}  // namespace configuration
}  // namespace database
}  // namespace artdaq

void cftd::SearchCollection() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);

  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TLOG(10) << "artdaq::database::configuration::SearchCollection trace_enable";
}
