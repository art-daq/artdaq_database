#include "artdaq-database/ConfigurationDB/common.h"

#include "artdaq-database/ConfigurationDB/dboperation_managealiases.h"

#include "artdaq-database/ConfigurationDB/shared_helper_functions.h"
#include "artdaq-database/DataFormats/shared_literals.h"
#include "artdaq-database/SharedCommon/configuraion_api_literals.h"

#include "artdaq-database/ConfigurationDB/configuration_dbproviders.h"

#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/DataFormats/Json/json_common.h"
#include "artdaq-database/DataFormats/shared_literals.h"

#include "artdaq-database/ConfigurationDB/options_operations.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "CONF:FndCfD_C"

namespace db = artdaq::database;
namespace cf = db::configuration;
namespace cftd = cf::debug::detail;
namespace jsonliteral = db::dataformats::literal;
namespace apiliteral = db::configapi::literal;

using cf::ManageDocumentOperation;
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

typedef JsonData (*provider_findversionaliases_t)(Options const& /*options*/, JsonData const& /*task_payload*/);
typedef JsonData (*provider_addversionalias_t)(Options const& /*options*/, JsonData const& /*task_payload*/);
typedef JsonData (*provider_rmversionalias_t)(Options const& /*options*/, JsonData const& /*task_payload*/);

void add_version_alias(Options const& options, std::string& configs) {
  confirm(configs.empty());
  confirm(options.operation().compare(apiliteral::operation::addversionalias) == 0);

  TRACE_(11, "add_version_alias: begin");
  TRACE_(11, "add_version_alias args options=<" << options.to_string() << ">");

  validate_dbprovider_name(options.provider());

  auto dispatch_persistence_provider = [](std::string const& name) {
    auto providers = std::map<std::string, provider_addversionalias_t>{
        {apiliteral::provider::mongo, cf::mongo::addAlias},
        {apiliteral::provider::filesystem, cf::filesystem::addAlias},
        {apiliteral::provider::ucon, cf::ucon::addAlias}};

    return providers.at(name);
  };

  auto search_result =
      dispatch_persistence_provider(options.provider())(options, options.query_filter_to_JsonData().json_buffer);

  auto returnValue = std::string{};
  auto returnValueChanged = bool{false};

  switch (options.format()) {
    default:
    case data_format_t::db:
    case data_format_t::json:
    case data_format_t::unknown:
    case data_format_t::fhicl: {
      throw runtime_error("add_version_alias") << "Unsupported data format.";
      break;
    }

    case data_format_t::gui: {
      returnValue = search_result.json_buffer;
      returnValueChanged = true;
      break;
    }
  }

  if (returnValueChanged) configs.swap(returnValue);

  TRACE_(11, "add_version_alias: end");
}

void remove_version_alias(Options const& options, std::string& configs) {
  confirm(configs.empty());
  confirm(options.operation().compare(apiliteral::operation::rmversionalias) == 0);

  TRACE_(11, "remove_version_alias: begin");
  TRACE_(11, "remove_version_alias args options=<" << options.to_string() << ">");

  validate_dbprovider_name(options.provider());

  auto dispatch_persistence_provider = [](std::string const& name) {
    auto providers = std::map<std::string, provider_rmversionalias_t>{
        {apiliteral::provider::mongo, cf::mongo::rmAlias},
        {apiliteral::provider::filesystem, cf::filesystem::rmAlias},
        {apiliteral::provider::ucon, cf::ucon::rmAlias}};

    return providers.at(name);
  };

  auto search_result =
      dispatch_persistence_provider(options.provider())(options, options.query_filter_to_JsonData().json_buffer);

  auto returnValue = std::string{};
  auto returnValueChanged = bool{false};

  switch (options.format()) {
    default:
    case data_format_t::db:
    case data_format_t::json:
    case data_format_t::unknown:
    case data_format_t::fhicl: {
      throw runtime_error("remove_version_alias") << "Unsupported data format.";
      break;
    }

    case data_format_t::gui: {
      returnValue = search_result.json_buffer;
      returnValueChanged = true;
      break;
    }
  }

  if (returnValueChanged) configs.swap(returnValue);

  TRACE_(11, "remove_version_alias: end");
}

void find_aliases(Options const& options, std::string& configs) {
  confirm(configs.empty());
  confirm(options.operation().compare(apiliteral::operation::findversionalias) == 0);

  TRACE_(11, "find_aliases: begin");
  TRACE_(11, "find_aliases args options=<" << options.to_string() << ">");

  validate_dbprovider_name(options.provider());

  auto dispatch_persistence_provider = [](std::string const& name) {
    auto providers = std::map<std::string, provider_findversionaliases_t>{
        {apiliteral::provider::mongo, cf::mongo::findAliases},
        {apiliteral::provider::filesystem, cf::filesystem::findAliases},
        {apiliteral::provider::ucon, cf::ucon::findAliases}};

    return providers.at(name);
  };

  auto search_result =
      dispatch_persistence_provider(options.provider())(options, options.query_filter_to_JsonData().json_buffer);

  auto returnValue = std::string{};
  auto returnValueChanged = bool{false};

  switch (options.format()) {
    default:
    case data_format_t::db:
    case data_format_t::json:
    case data_format_t::unknown:
    case data_format_t::fhicl:
    case data_format_t::xml: {
      if (!db::json_db_to_gui(search_result.json_buffer, returnValue)) {
        throw runtime_error("find_aliases") << "Unsupported data format.";
      }
      break;
    }

    case data_format_t::gui: {
      returnValue = search_result.json_buffer;
      returnValueChanged = true;
      break;
    }

    case data_format_t::csv: {
      using namespace artdaq::database::json;
      auto reader = JsonReader{};
      object_t results_ast;

      if (!reader.read(search_result.json_buffer, results_ast)) {
        TRACE_(11, "find_aliases() Failed to create an AST from search results JSON.");

        throw runtime_error("find_aliases") << "Failed to create an AST from search results JSON.";
      }

      auto const& results_list = boost::get<array_t>(results_ast.at(jsonliteral::search));

      TRACE_(11, "find_aliases: found " << results_list.size() << " results.");

      std::ostringstream os;

      for (auto const& result_entry : results_list) {
        auto const& buff = boost::get<object_t>(result_entry).at(apiliteral::name);
        auto value = boost::apply_visitor(jsn::tostring_visitor(), buff);

        TRACE_(11, "find_aliases() Found config=<" << value << ">.");

        os << value << ", ";
      }
      returnValue = os.str();
      returnValueChanged = true;
      break;
    }
  }

  if (returnValueChanged) configs.swap(returnValue);

  TRACE_(11, "find_aliases: end");
}
}  // namespace detail
}  // namespace configuration
}  // namespace database
}  // namespace artdaq


void cftd::enableManageAliases() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);

  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TRACE_(0, "artdaq::database::configuration::enableManageAliases trace_enable");
}
