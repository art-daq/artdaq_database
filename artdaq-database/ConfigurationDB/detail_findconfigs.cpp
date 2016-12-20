#include "artdaq-database/ConfigurationDB/common.h"

#include "artdaq-database/ConfigurationDB/dboperation_manageconfigs.h"

#include "artdaq-database/ConfigurationDB/shared_helper_functions.h"
#include "artdaq-database/ConfigurationDB/shared_literals.h"

#include "artdaq-database/ConfigurationDB/configuration_dbproviders.h"

#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/DataFormats/Json/json_common.h"
#include "artdaq-database/DataFormats/common/shared_literals.h"

#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/JsonDocument/JSONDocument_template.h"

#include <boost/exception/diagnostic_information.hpp>

#include "artdaq-database/BuildInfo/process_exit_codes.h"
#include "artdaq-database/ConfigurationDB/options_operations.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "CONF:FndCfD_C"

namespace db = artdaq::database;
namespace cf = db::configuration;
namespace cfl = cf::literal;
namespace cflo = cfl::operation;
namespace cflp = cfl::provider;
namespace cftd = cf::debug::detail;

using cf::ManageConfigsOperation;
using cf::options::data_format_t;

using Options = cf::ManageConfigsOperation;

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

namespace artdaq {
namespace database {
namespace configuration {
namespace detail {

typedef JsonData (*provider_findglobalconfigs_t)(Options const& /*options*/, JsonData const& /*search_filter*/);
typedef JsonData (*provider_buildconfigsearchfilter_t)(Options const& /*options*/, JsonData const& /*search_filter*/);

void find_global_configurations(Options const& options, std::string& configs) {
  assert(configs.empty());
  assert(options.operation().compare(cflo::findconfigs) == 0);

  TRACE_(11, "find_global_configurations: begin");
  TRACE_(11, "find_global_configurations args options=<" << options.to_string() << ">");

  validate_dbprovider_name(options.provider());

  auto dispatch_persistence_provider = [](std::string const& name) {
    auto providers = std::map<std::string, provider_findglobalconfigs_t>{{cflp::mongo, cf::mongo::findGlobalConfigs},
                                                                         {cflp::filesystem, cf::filesystem::findGlobalConfigs},
                                                                         {cflp::ucond, cf::ucond::findGlobalConfigs}};

    return providers.at(name);
  };

  auto search_result = dispatch_persistence_provider(options.provider())(options, options.search_filter_to_JsonData().json_buffer);

  auto returnValue = std::string{};
  auto returnValueChanged = bool{false};

  switch (options.dataFormat()) {
    default:
    case data_format_t::db:
    case data_format_t::json:
    case data_format_t::unknown:
    case data_format_t::fhicl:
    case data_format_t::xml: {
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
void build_global_configuration_search_filter(Options const& options, std::string& filters) {
  assert(filters.empty());
  assert(options.operation().compare(cflo::buildfilter) == 0);

  TRACE_(12, "build_global_configuration_search_filter: begin");
  TRACE_(11, "build_global_configuration_search_filter args options=<" << options.to_string() << ">");

  validate_dbprovider_name(options.provider());

  auto dispatch_persistence_provider = [](std::string const& name) {
    auto providers = std::map<std::string, provider_buildconfigsearchfilter_t>{{cflp::mongo, cf::mongo::buildConfigSearchFilter},
                                                                               {cflp::filesystem, cf::filesystem::buildConfigSearchFilter},
                                                                               {cflp::ucond, cf::ucond::buildConfigSearchFilter}};

    return providers.at(name);
  };

  auto search_result = dispatch_persistence_provider(options.provider())(options, options.search_filter_to_JsonData().json_buffer);

  auto returnValue = std::string{};
  auto returnValueChanged = bool{false};

  switch (options.dataFormat()) {
    default:
    case data_format_t::db:
    case data_format_t::json:
    case data_format_t::unknown:
    case data_format_t::fhicl:
    case data_format_t::xml: {
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

  TRACE_(11, "build_global_configuration_search_filter: end");
}
}  // namespace detail
}  // namespace configuration
}  // namespace database
}  // namespace artdaq

void cftd::enableFindConfigsOperation() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);

  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TRACE_(0, "artdaq::database::configuration::FindConfigsDetail trace_enable");
}
