#include "artdaq-database/ConfigurationDB/common.h"

#include "artdaq-database/ConfigurationDB/dboperation_filedb.h"
#include "artdaq-database/ConfigurationDB/dboperation_findconfigs.h"
#include "artdaq-database/ConfigurationDB/dboperation_mongodb.h"

#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/FhiclJson/shared_literals.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/JsonDocument/JSONDocument_template.h"

#include <boost/exception/diagnostic_information.hpp>

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "CONF:OpSrchD_C"

namespace db = artdaq::database;
namespace dbjsu = db::jsonutils;
namespace dbjsul = db::jsonutils::literal;
namespace cf = db::configuration;
namespace cfgui = cf::guiexports;
namespace cfo = cf::options;
namespace cfol = cfo::literal;

using cfo::FindConfigsOperation;
using cfo::data_format_t;

using Options = cfo::FindConfigsOperation;

using artdaq::database::basictypes::JsonData;
using artdaq::database::basictypes::FhiclData;
using artdaq::database::jsonutils::JSONDocument;
using artdaq::database::jsonutils::JSONDocumentBuilder;

namespace artdaq {
namespace database {
bool json_db_to_gui(std::string const&, std::string&);
bool json_gui_to_db(std::string const&, std::string&);
}
}

namespace detail {

typedef JsonData (*provider_findglobalconfigs_t)(Options const& /*options*/, JsonData const& /*search_filter*/);
typedef JsonData (*provider_buildconfigsearchfilter_t)(Options const& /*options*/, JsonData const& /*search_filter*/);

void find_global_configurations(FindConfigsOperation const& options, std::string& configs) {
  assert(configs.empty());
  assert(options.operation().compare(cfo::literal::operation_findconfigs) == 0);

  TRACE_(11, "find_global_configurations: begin");
  TRACE_(11, "find_global_configurations args options=<" << options.to_string() << ">");

  if (cf::not_equal(options.provider(), cfol::database_provider_filesystem) &&
      cf::not_equal(options.provider(), cfol::database_provider_mongo)) {
    TRACE_(11, "Error in find_global_configurations:"
                   << " Invalid database provider; database provider=" << options.provider() << ".");

    throw cet::exception("find_global_configurations")
        << "Invalid database provider; database provider=" << options.provider() << ".";
  }

  auto dispatch_persistence_provider = [](std::string const& name) {
    auto providers = std::map<std::string, provider_findglobalconfigs_t>{
        {cfol::database_provider_mongo, cf::mongo::findGlobalConfigs},
        {cfol::database_provider_filesystem, cf::filesystem::findGlobalConfigs}};

    return providers.at(name);
  };

  auto search_result =
      dispatch_persistence_provider(options.provider())(options, options.search_filter_jsondoc().to_string());

  auto returnValue = std::string{};
  auto returnValueChanged = bool{false};

  switch (options.dataFormat()) {
    default:
    case data_format_t::db:
    case data_format_t::json:
    case data_format_t::unknown:
    case data_format_t::fhicl: {
      if (!db::json_db_to_gui(search_result.json_buffer, returnValue)) {
        throw cet::exception("find_global_configurations") << "Unsupported data format.";
      }
      break;
    }

    case data_format_t::gui: {
      returnValue = search_result.json_buffer;
      returnValueChanged = true;
      break;
    }
  }

  if (returnValueChanged) configs.swap(returnValue);

  TRACE_(11, "find_global_configurations: end");
}
void build_global_configuration_search_filter(FindConfigsOperation const& options, std::string& filters) {
  assert(filters.empty());
  assert(options.operation().compare(cfo::literal::operation_buildfilter) == 0);

  TRACE_(12, "build_global_configuration_search_filter: begin");
  TRACE_(11, "build_global_configuration_search_filter args options=<" << options.to_string() << ">");

  if (cf::not_equal(options.provider(), cfol::database_provider_filesystem) &&
      cf::not_equal(options.provider(), cfol::database_provider_mongo)) {
    TRACE_(11, "Error in build_global_configuration_search_filter:"
                   << " Invalid database provider; database provider=" << options.provider() << ".");

    throw cet::exception("build_global_configuration_search_filter")
        << "Invalid database provider; database provider=" << options.provider() << ".";
  }

  auto dispatch_persistence_provider = [](std::string const& name) {
    auto providers = std::map<std::string, provider_buildconfigsearchfilter_t>{
        {cfol::database_provider_mongo, cf::mongo::buildConfigSearchFilter},
        {cfol::database_provider_filesystem, cf::filesystem::buildConfigSearchFilter}};

    return providers.at(name);
  };

  auto search_result =
      dispatch_persistence_provider(options.provider())(options, options.search_filter_jsondoc().to_string());

  auto returnValue = std::string{};
  auto returnValueChanged = bool{false};

  switch (options.dataFormat()) {
    default:
    case data_format_t::db:
    case data_format_t::json:
    case data_format_t::unknown:
    case data_format_t::fhicl: {
      if (!db::json_db_to_gui(search_result.json_buffer, returnValue)) {
        throw cet::exception("build_global_configuration_search_filter") << "Unsupported data format.";
      }
      break;
    }

    case data_format_t::gui: {
      returnValue = search_result.json_buffer;
      returnValueChanged = true;
      break;
    }
  }

  if (returnValueChanged) filters.swap(returnValue);

  TRACE_(11, "build_global_configuration_search_filter: end");}
}

void cf::trace_enable_FindConfigsOperationDetail() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", 1LL);
  TRACE_CNTL("modeS", 1LL);

  TRACE_(0, "artdaq::database::configuration::FindConfigsOperationDetail"
                << "trace_enable");
}
