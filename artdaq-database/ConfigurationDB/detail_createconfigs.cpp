#include "artdaq-database/ConfigurationDB/common.h"

#include "artdaq-database/ConfigurationDB/dboperation_manageconfigs.h"

#include "artdaq-database/ConfigurationDB/shared_helper_functions.h"
#include "artdaq-database/ConfigurationDB/shared_literals.h"

#include "artdaq-database/ConfigurationDB/dispatch_filedb.h"
#include "artdaq-database/ConfigurationDB/dispatch_mongodb.h"

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

#define TRACE_NAME "CONF:CrtCfD_C"

namespace db = artdaq::database;
namespace cf = db::configuration;
namespace cfl = cf::literal;
namespace cflo = cfl::operation;
namespace cflp = cfl::provider;
namespace cftd = cf::debug::detail;

using cf::LoadStoreOperation;
using cf::options::data_format_t;

using Options = cf::LoadStoreOperation;

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

typedef JsonData (*provider_findversions_t)(Options const& /*options*/, JsonData const& /*search_filter*/);
typedef JsonData (*provider_findentities_t)(Options const& /*options*/, JsonData const& /*search_filter*/);
typedef JsonData (*provider_addtoglobalconfig_t)(Options const& /*options*/, JsonData const& /*search_filter*/);

void add_configuration_to_global_configuration(Options const& options, std::string& configs) {
  assert(configs.empty());
  assert(options.operation().compare(cflo::addconfig) == 0);

  TRACE_(11, "add_configuration_to_global_configuration: begin"
                 << "aq");
  TRACE_(11, "add_configuration_to_global_configuration args options=<" << options.to_string() << ">");

  if (cf::not_equal(options.provider(), cflp::filesystem) && cf::not_equal(options.provider(), cflp::mongo)) {
    TRACE_(11, "Error in add_configuration_to_global_configuration:"
                   << " Invalid database provider; database provider=" << options.provider() << ".");

    throw cet::exception("add_configuration_to_global_configuration")
        << "Invalid database provider; database provider=" << options.provider() << ".";
  }

  auto dispatch_persistence_provider = [](std::string const& name) {
    auto providers = std::map<std::string, provider_addtoglobalconfig_t>{
        {cflp::mongo, cf::mongo::addConfigToGlobalConfig}, {cflp::filesystem, cf::filesystem::addConfigToGlobalConfig}};

    return providers.at(name);
  };

  auto search_result =
      dispatch_persistence_provider(options.provider())(options, options.search_filter_to_JsonData().json_buffer);

  auto returnValue = std::string{};
  auto returnValueChanged = bool{false};

  switch (options.dataFormat()) {
    default:
    case data_format_t::db:
    case data_format_t::json:
    case data_format_t::unknown:
    case data_format_t::fhicl: {
      throw cet::exception("find_configuration_versions") << "Unsupported data format.";
      break;
    }

    case data_format_t::gui: {
      returnValue = search_result.json_buffer;
      returnValueChanged = true;
      break;
    }
  }

  if (returnValueChanged) configs.swap(returnValue);

  TRACE_(11, "add_configuration_to_global_configuration: end");
}

void create_new_global_configuration(std::string const& operations, std::string& configs) {
  assert(!operations.empty());
  assert(configs.empty());

  using namespace artdaq::database::json;

  TRACE_(11, "create_new_global_configuration: begin");
  TRACE_(11, "create_new_global_configuration args operations=<" << operations << ">");

  auto reader = JsonReader{};
  object_t operations_ast;

  if (!reader.read(operations, operations_ast)) {
    TRACE_(11, "create_new_global_configuration() Failed to create an AST from operations JSON.");

    throw cet::exception("create_new_global_configuration") << "Failed to create an AST from operations JSON.";
  }

  auto const& operations_list = boost::get<array_t>(operations_ast.at(cfl::gui::operations));

  TRACE_(11, "create_new_global_configuration: found " << operations_list.size() << " operations.");

  for (auto const& operation_entry : operations_list) {
    auto buff = std::string{};
    JsonWriter{}.write(boost::get<object_t>(operation_entry), buff);

    TRACE_(11, "create_new_global_configuration() Found operation=<" << buff << ">.");

    auto addconfig = Options{cfl::gui::operations};
    addconfig.readJsonData(buff);

    configs.clear();

    add_configuration_to_global_configuration(addconfig, configs);
  }

  TRACE_(11, "create_new_global_configuration: end");
}

void find_configuration_versions(Options const& options, std::string& versions) {
  assert(versions.empty());
  assert(options.operation().compare(cflo::findversions) == 0);

  TRACE_(11, "find_configuration_versions: begin");
  TRACE_(11, "find_configuration_versions args options=<" << options.to_string() << ">");

  if (cf::not_equal(options.provider(), cflp::filesystem) && cf::not_equal(options.provider(), cflp::mongo)) {
    TRACE_(11, "Error in find_configuration_versions:"
                   << " Invalid database provider; database provider=" << options.provider() << ".");

    throw cet::exception("find_configuration_versions")
        << "Invalid database provider; database provider=" << options.provider() << ".";
  }

  auto dispatch_persistence_provider = [](std::string const& name) {
    auto providers = std::map<std::string, provider_findversions_t>{
        {cflp::mongo, cf::mongo::findConfigVersions}, {cflp::filesystem, cf::filesystem::findConfigVersions}};

    return providers.at(name);
  };

  auto search_result =
      dispatch_persistence_provider(options.provider())(options, options.search_filter_to_JsonData().json_buffer);

  auto returnValue = std::string{};
  auto returnValueChanged = bool{false};

  switch (options.dataFormat()) {
    default:
    case data_format_t::db:
    case data_format_t::json:
    case data_format_t::unknown:
    case data_format_t::fhicl: {
      throw cet::exception("find_configuration_versions") << "Unsupported data format.";
      break;
    }

    case data_format_t::gui: {
      returnValue = search_result.json_buffer;
      returnValueChanged = true;
      break;
    }
  }

  if (returnValueChanged) versions.swap(returnValue);

  TRACE_(11, "find_configuration_versions: end");
}

void find_configuration_entities(Options const& options, std::string& entities) {
  assert(entities.empty());
  assert(options.operation().compare(cflo::findentities) == 0);

  TRACE_(11, "find_configuration_entities: begin");
  TRACE_(11, "find_configuration_entities args options=<" << options.to_string() << ">");

  if (cf::not_equal(options.provider(), cflp::filesystem) && cf::not_equal(options.provider(), cflp::mongo)) {
    TRACE_(11, "Error in find_configuration_entities:"
                   << " Invalid database provider; database provider=" << options.provider() << ".");

    throw cet::exception("find_configuration_entities")
        << "Invalid database provider; database provider=" << options.provider() << ".";
  }

  auto dispatch_persistence_provider = [](std::string const& name) {
    auto providers = std::map<std::string, provider_findentities_t>{
        {cflp::mongo, cf::mongo::findConfigEntities}, {cflp::filesystem, cf::filesystem::findConfigEntities}};

    return providers.at(name);
  };

  auto search_result =
      dispatch_persistence_provider(options.provider())(options, options.search_filter_to_JsonData().json_buffer);

  auto returnValue = std::string{};
  auto returnValueChanged = bool{false};

  switch (options.dataFormat()) {
    default:
    case data_format_t::db:
    case data_format_t::json:
    case data_format_t::unknown:
    case data_format_t::fhicl: {
      throw cet::exception("find_configuration_entities") << "Unsupported data format.";
      break;
    }

    case data_format_t::gui: {
      returnValue = search_result.json_buffer;
      returnValueChanged = true;
      break;
    }
  }

  if (returnValueChanged) entities.swap(returnValue);

  TRACE_(11, "find_configuration_entities: end");
  TRACE_(11, "find_configuration_entities: end entities=" << entities);
}
}  // namespace detail
}  // namespace configuration
}  // namespace database
}  // namespace artdaq

void cftd::enableCreateConfigsOperation() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);

  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TRACE_(0, "artdaq::database::configuration::CreateConfigsOperationDetail trace_enable");
}
