#include "artdaq-database/ConfigurationDB/common.h"

#include "artdaq-database/ConfigurationDB/dboperation_manageconfigs.h"

#include "artdaq-database/ConfigurationDB/shared_helper_functions.h"
#include "artdaq-database/DataFormats/shared_literals.h"
#include "artdaq-database/SharedCommon/configuraion_api_literals.h"

#include "artdaq-database/ConfigurationDB/configuration_dbproviders.h"

#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/DataFormats/Json/json_common.h"
#include "artdaq-database/DataFormats/shared_literals.h"

#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/JsonDocument/docrecord_literals.h"

#include "artdaq-database/ConfigurationDB/options_operations.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "detail_manageconfigs.cpp"

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

using provider_call_returnslist_t = std::vector<JSONDocument> (*)(const Options&, const JSONDocument&);
using provider_call_t = JSONDocument (*)(const Options&, const JSONDocument&);

void assign_configuration(Options const& options, std::string& configs) {
  confirm(configs.empty());
  confirm(options.operation() == apiliteral::operation::assignconfig);

  TLOG(21) << "assign_configuration: begin";
  TLOG(21) << "assign_configuration args options=<" << options << ">";

  validate_dbprovider_name(options.provider());

  auto dispatch_persistence_provider = [](std::string const& name) {
    auto providers = std::map<std::string, provider_call_t>{{apiliteral::provider::mongo, cf::mongo::assignConfiguration},
                                                            {apiliteral::provider::filesystem, cf::filesystem::assignConfiguration},
                                                            {apiliteral::provider::ucon, cf::ucon::assignConfiguration}};

    return providers.at(name);
  };

  auto search_result = dispatch_persistence_provider(options.provider())(options, options.query_filter_to_JsonData());

  auto returnValue = std::string{};
  auto returnValueChanged = bool{false};

  switch (options.format()) {
    default:
    case data_format_t::db:
    case data_format_t::json:
    case data_format_t::unknown:
    case data_format_t::fhicl: {
      throw runtime_error("assign_configuration") << "Unsupported data format.";
      break;
    }

    case data_format_t::gui: {
      returnValue = search_result;
      returnValueChanged = true;
      break;
    }
  }

  if (returnValueChanged) {
    configs.swap(returnValue);
  }

  TLOG(21) << "assign_configuration: end";
}

void remove_configuration(Options const& options, std::string& configs) {
  confirm(configs.empty());
  confirm(options.operation() == apiliteral::operation::removeconfig);

  TLOG(21) << "remove_configuration: begin";
  TLOG(21) << "remove_configuration args options=<" << options << ">";

  validate_dbprovider_name(options.provider());

  auto dispatch_persistence_provider = [](std::string const& name) {
    auto providers = std::map<std::string, provider_call_t>{{apiliteral::provider::mongo, cf::mongo::removeConfiguration},
                                                            {apiliteral::provider::filesystem, cf::filesystem::removeConfiguration},
                                                            {apiliteral::provider::ucon, cf::ucon::removeConfiguration}};

    return providers.at(name);
  };

  auto search_result = dispatch_persistence_provider(options.provider())(options, options.query_filter_to_JsonData());

  auto returnValue = std::string{};
  auto returnValueChanged = bool{false};

  switch (options.format()) {
    default:
    case data_format_t::db:
    case data_format_t::json:
    case data_format_t::unknown:
    case data_format_t::fhicl: {
      throw runtime_error("remove_configuration") << "Unsupported data format.";
      break;
    }

    case data_format_t::gui: {
      returnValue = search_result;
      returnValueChanged = true;
      break;
    }
  }

  if (returnValueChanged) {
    configs.swap(returnValue);
  }

  TLOG(21) << "remove_configuration: end";
}
void create_configuration(std::string const& operations, std::string& configs) {
  confirm(!operations.empty());
  confirm(configs.empty());

  using namespace artdaq::database::json;

  TLOG(21) << "create_configuration: begin";
  TLOG(21) << "create_configuration args operations=<" << operations << ">";

  auto reader = JsonReader{};
  object_t operations_ast;

  if (!reader.read(operations, operations_ast)) {
    TLOG(21) << "create_configuration() Failed to create an AST from operations JSON.";

    throw runtime_error("create_configuration") << "Failed to create an AST from operations JSON.";
  }

  auto const& operations_list = boost::get<array_t>(operations_ast.at(apiliteral::operations));

  TLOG(21) << "create_configuration: found " << operations_list.size() << " operations.";

  for (auto const& operation_entry : operations_list) {
    auto buff = std::string{};
    JsonWriter{}.write(boost::get<object_t>(operation_entry), buff);

    TLOG(21) << "create_configuration() Found operation=<" << buff << ">.";

    auto addconfig = Options{apiliteral::operations};
    addconfig.readJsonData(buff);

    configs.clear();

    assign_configuration(addconfig, configs);
  }

  TLOG(21) << "create_configuration: end";
}

void find_configurations(Options const& options, std::string& configs) {
  confirm(configs.empty());
  confirm(options.operation() == apiliteral::operation::findconfigs);

  TLOG(21) << "find_configurations: begin";
  TLOG(21) << "find_configurations args options=<" << options << ">";

  validate_dbprovider_name(options.provider());

  auto dispatch_persistence_provider = [](std::string const& name) {
    auto providers = std::map<std::string, provider_call_returnslist_t>{{apiliteral::provider::mongo, cf::mongo::findConfigurations},
                                                                        {apiliteral::provider::filesystem, cf::filesystem::findConfigurations},
                                                                        {apiliteral::provider::ucon, cf::ucon::findConfigurations}};

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
      throw runtime_error("find_configurations") << "Unsupported data format.";
      break;
    }

    case data_format_t::gui: {
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

    case data_format_t::csv: {
      std::ostringstream oss;
      for (auto const& search_result : search_results) {
        oss << search_result.value_as<std::string>(apiliteral::name) << ",";
      }

      returnValue = oss.str();

      if (returnValue.back() == ',') {
        returnValue.pop_back();
      }

      returnValueChanged = true;
      break;
    }
  }

  if (returnValueChanged) {
    configs.swap(returnValue);
  }

  TLOG(21) << "find_configurations: end";
}
void configuration_composition(Options const& options, std::string& filters) {
  confirm(filters.empty());
  confirm(options.operation() == apiliteral::operation::confcomposition);

  TLOG(22) << "configuration_composition: begin";
  TLOG(21) << "configuration_composition args options=<" << options << ">";

  validate_dbprovider_name(options.provider());

  auto dispatch_persistence_provider = [](std::string const& name) {
    auto providers = std::map<std::string, provider_call_t>{{apiliteral::provider::mongo, cf::mongo::configurationComposition},
                                                            {apiliteral::provider::filesystem, cf::filesystem::configurationComposition},
                                                            {apiliteral::provider::ucon, cf::ucon::configurationComposition}};

    return providers.at(name);
  };

  auto search_result = dispatch_persistence_provider(options.provider())(options, options.query_filter_to_JsonData());

  auto returnValue = std::string{};
  auto returnValueChanged = bool{false};

  switch (options.format()) {
    default:
    case data_format_t::db:
    case data_format_t::json:
    case data_format_t::unknown:
    case data_format_t::fhicl:
    case data_format_t::xml: {
      if (!db::json_db_to_gui(search_result, returnValue)) {
        throw runtime_error("configuration_composition") << "Unsupported data format.";
      }
      break;
    }

    case data_format_t::gui: {
      returnValue = search_result;
      returnValueChanged = true;
      break;
    }
  }

  if (returnValueChanged) {
    filters.swap(returnValue);
  }

  TLOG(21) << "configuration_composition: end";
}
}  // namespace detail
}  // namespace configuration
}  // namespace database
}  // namespace artdaq
void cftd::ManageConfigs() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);

  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TLOG(10) << "artdaq::database::configuration::ManageConfigs trace_enable";
}
