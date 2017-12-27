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

#define TRACE_NAME "CONF:CrtCfD_C"

namespace db = artdaq::database;
namespace cf = db::configuration;
namespace cftd = cf::debug::detail;
namespace jsonliteral = db::dataformats::literal;
namespace apiliteral = db::configapi::literal;

using cf::options::data_format_t;

using Options = cf::ManageDocumentOperation;

using artdaq::database::basictypes::JsonData;
using artdaq::database::basictypes::FhiclData;
using artdaq::database::docrecord::JSONDocument;
using artdaq::database::docrecord::JSONDocumentBuilder;

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

typedef JsonData (*provider_call_t)(Options const& /*options*/, JsonData const& /*task_payload*/);

void assign_configuration(Options const& options, std::string& configs) {
  confirm(configs.empty());
  confirm(options.operation().compare(apiliteral::operation::assignconfig) == 0);

  TLOG(11)<< "assign_configuration: begin";
  TLOG(11)<< "assign_configuration args options=<" << options << ">";

  validate_dbprovider_name(options.provider());

  auto dispatch_persistence_provider = [](std::string const& name) {
    auto providers =
        std::map<std::string, provider_call_t>{{apiliteral::provider::mongo, cf::mongo::assignConfiguration},
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

  if (returnValueChanged) configs.swap(returnValue);

  TLOG(11)<< "assign_configuration: end";
}

void remove_configuration(Options const& options, std::string& configs) {
  confirm(configs.empty());
  confirm(options.operation().compare(apiliteral::operation::removeconfig) == 0);

  TLOG(11)<< "remove_configuration: begin";
  TLOG(11)<< "remove_configuration args options=<" << options << ">";

  validate_dbprovider_name(options.provider());

  auto dispatch_persistence_provider = [](std::string const& name) {
    auto providers =
        std::map<std::string, provider_call_t>{{apiliteral::provider::mongo, cf::mongo::removeConfiguration},
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

  if (returnValueChanged) configs.swap(returnValue);

  TLOG(11)<< "remove_configuration: end";
}
void create_configuration(std::string const& operations, std::string& configs) {
  confirm(!operations.empty());
  confirm(configs.empty());

  using namespace artdaq::database::json;

  TLOG(11)<< "create_configuration: begin";
  TLOG(11)<< "create_configuration args operations=<" << operations << ">";

  auto reader = JsonReader{};
  object_t operations_ast;

  if (!reader.read(operations, operations_ast)) {
    TLOG(11)<<
           "create_configuration() Failed to create an AST from operations JSON.";

    throw runtime_error("create_configuration") << "Failed to create an AST from operations JSON.";
  }

  auto const& operations_list = boost::get<array_t>(operations_ast.at(apiliteral::operations));

  TLOG(11)<< "create_configuration: found " << operations_list.size() << " operations.";

  for (auto const& operation_entry : operations_list) {
    auto buff = std::string{};
    JsonWriter{}.write(boost::get<object_t>(operation_entry), buff);

    TLOG(11)<< "create_configuration() Found operation=<" << buff << ">.";

    auto addconfig = Options{apiliteral::operations};
    addconfig.readJsonData(buff);

    configs.clear();

    assign_configuration(addconfig, configs);
  }

  TLOG(11)<< "create_configuration: end";
}

void find_configurations(Options const& options, std::string& configs) {
  confirm(configs.empty());
  confirm(options.operation().compare(apiliteral::operation::findconfigs) == 0);

  TLOG(11)<< "find_configurations: begin";
  TLOG(11)<< "find_configurations args options=<" << options << ">";

  validate_dbprovider_name(options.provider());

  auto dispatch_persistence_provider = [](std::string const& name) {
    auto providers =
        std::map<std::string, provider_call_t>{{apiliteral::provider::mongo, cf::mongo::findConfigurations},
                                               {apiliteral::provider::filesystem, cf::filesystem::findConfigurations},
                                               {apiliteral::provider::ucon, cf::ucon::findConfigurations}};

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
        throw runtime_error("find_configurations") << "Unsupported data format.";
      }
      break;
    }

    case data_format_t::gui: {
      returnValue = search_result;
      returnValueChanged = true;
      break;
    }

    case data_format_t::csv: {
      using namespace artdaq::database::json;
      auto reader = JsonReader{};
      object_t results_ast;

      if (!reader.read(search_result, results_ast)) {
        TLOG(11)<< "find_configurations() Failed to create an AST from search results JSON.";

        throw runtime_error("find_configurations") << "Failed to create an AST from search results JSON.";
      }

      auto const& results_list = boost::get<array_t>(results_ast.at(jsonliteral::search));

      TLOG(11)<< "find_configurations: found " << results_list.size() << " results.";

      std::ostringstream os;

      for (auto const& result_entry : results_list) {
        auto const& buff = boost::get<object_t>(result_entry).at(apiliteral::name);
        auto value = boost::apply_visitor(jsn::tostring_visitor(), buff);

        TLOG(11)<< "find_configurations() Found config=<" << value << ">.";

        os << value << ",";
      }
      returnValue = os.str();

      if(returnValue.back()==',')
	returnValue.pop_back();
      
      returnValueChanged = true;
      break;
    }
  }

  if (returnValueChanged) configs.swap(returnValue);

  TLOG(11)<< "find_configurations: end";
}
void configuration_composition(Options const& options, std::string& filters) {
  confirm(filters.empty());
  confirm(options.operation().compare(apiliteral::operation::confcomposition) == 0);

  TLOG(12) << "configuration_composition: begin";
  TLOG(11)<< "configuration_composition args options=<" << options << ">";

  validate_dbprovider_name(options.provider());

  auto dispatch_persistence_provider = [](std::string const& name) {
    auto providers = std::map<std::string, provider_call_t>{
        {apiliteral::provider::mongo, cf::mongo::configurationComposition},
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

  if (returnValueChanged) filters.swap(returnValue);

  TLOG(11)<< "configuration_composition: end";
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

  TLOG(0) <<  "artdaq::database::configuration::ManageConfigs trace_enable";
}
