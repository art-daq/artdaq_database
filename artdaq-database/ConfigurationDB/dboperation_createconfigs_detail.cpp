#include "artdaq-database/ConfigurationDB/common.h"

#include "artdaq-database/ConfigurationDB/dboperation_createconfigs.h"
#include "artdaq-database/ConfigurationDB/dboperation_filedb.h"
#include "artdaq-database/ConfigurationDB/dboperation_loadstore.h"

#include "artdaq-database/ConfigurationDB/dboperation_mongodb.h"

#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/FhiclJson/shared_literals.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/JsonDocument/JSONDocument_template.h"

#include <boost/exception/diagnostic_information.hpp>

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "CONF:OpCrGCoD_C"

namespace db = artdaq::database;
namespace dbjsu = db::jsonutils;
namespace dbjsul = db::jsonutils::literal;
namespace cf = db::configuration;
namespace cfgui = cf::guiexports;
namespace cfo = cf::options;
namespace cfol = cfo::literal;

using cfo::LoadStoreOperation;
using cfo::data_format_t;

using Options = cfo::LoadStoreOperation;

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

typedef JsonData (*provider_findversions_t)(Options const& /*options*/, JsonData const& /*search_filter*/);
typedef JsonData (*provider_findentities_t)(Options const& /*options*/, JsonData const& /*search_filter*/);
typedef JsonData (*provider_newglobalconfig_t)(Options const& /*options*/, JsonData const& /*search_filter*/);
typedef JsonData (*provider_addtoglobalconfig_t)(Options const& /*options*/, JsonData const& /*search_filter*/);

void add_configuration_to_global_configuration(LoadStoreOperation const& options, std::string& configs) {
  assert(configs.empty());
  assert(options.operation().compare(cfo::literal::operation_addconfig) == 0);

  TRACE_(11, "add_configuration_to_global_configuration: begin");
  TRACE_(11, "add_configuration_to_global_configuration args options=<" << options.to_string() << ">");

  if (cf::not_equal(options.provider(), cfol::database_provider_filesystem) &&
      cf::not_equal(options.provider(), cfol::database_provider_mongo)) {
    TRACE_(11, "Error in add_configuration_to_global_configuration:"
                   << " Invalid database provider; database provider=" << options.provider() << ".");

    throw cet::exception("add_configuration_to_global_configuration")
        << "Invalid database provider; database provider=" << options.provider() << ".";
  }

  auto dispatch_persistence_provider = [](std::string const& name) {
    auto providers = std::map<std::string, provider_addtoglobalconfig_t>{
        {cfol::database_provider_mongo, cf::mongo::addConfigToGlobalConfig},
        {cfol::database_provider_filesystem, cf::filesystem::addConfigToGlobalConfig}};

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

void create_new_global_configuration(LoadStoreOperation const& options, std::string& configs[[gnu::unused]]) {
  assert(configs.empty());
  assert(options.operation().compare(cfo::literal::operation_newconfig) == 0);

  TRACE_(11, "create_new_global_configuration: begin");
  TRACE_(11, "create_new_global_configuration args options=<" << options.to_string() << ">");

  if (cf::not_equal(options.provider(), cfol::database_provider_filesystem) &&
      cf::not_equal(options.provider(), cfol::database_provider_mongo)) {
    TRACE_(11, "Error in create_new_global_configuration:"
                   << " Invalid database provider; database provider=" << options.provider() << ".");

    throw cet::exception("create_new_global_configuration")
        << "Invalid database provider; database provider=" << options.provider() << ".";
  }

  TRACE_(11, "create_new_global_configuration: end");
  throw cet::exception("create_new_global_configuration") << "Not Implemented. ";
}

void find_configuration_versions(LoadStoreOperation const& options, std::string& versions) {
  assert(versions.empty());
  assert(options.operation().compare(cfo::literal::operation_findversions) == 0);

  TRACE_(11, "find_configuration_versions: begin");
  TRACE_(11, "find_configuration_versions args options=<" << options.to_string() << ">");

  if (cf::not_equal(options.provider(), cfol::database_provider_filesystem) &&
      cf::not_equal(options.provider(), cfol::database_provider_mongo)) {
    TRACE_(11, "Error in find_configuration_versions:"
                   << " Invalid database provider; database provider=" << options.provider() << ".");

    throw cet::exception("find_configuration_versions")
        << "Invalid database provider; database provider=" << options.provider() << ".";
  }

  auto dispatch_persistence_provider = [](std::string const& name) {
    auto providers = std::map<std::string, provider_findversions_t>{
        {cfol::database_provider_mongo, cf::mongo::findConfigVersions},
        {cfol::database_provider_filesystem, cf::filesystem::findConfigVersions}};

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

void find_configuration_entities(LoadStoreOperation const& options, std::string& entities) {
  assert(entities.empty());
  assert(options.operation().compare(cfo::literal::operation_findentities) == 0);

  TRACE_(11, "find_configuration_entities: begin");
  TRACE_(11, "find_configuration_entities args options=<" << options.to_string() << ">");

  if (cf::not_equal(options.provider(), cfol::database_provider_filesystem) &&
      cf::not_equal(options.provider(), cfol::database_provider_mongo)) {
    TRACE_(11, "Error in find_configuration_entities:"
                   << " Invalid database provider; database provider=" << options.provider() << ".");

    throw cet::exception("find_configuration_entities")
        << "Invalid database provider; database provider=" << options.provider() << ".";
  }

  auto dispatch_persistence_provider = [](std::string const& name) {
    auto providers = std::map<std::string, provider_findentities_t>{
        {cfol::database_provider_mongo, cf::mongo::findConfigEntities},
        {cfol::database_provider_filesystem, cf::filesystem::findConfigEntities}};

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
}


void cf::trace_enable_CreateConfigsOperationDetail() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", 1LL);
  TRACE_CNTL("modeS", 1LL);

  TRACE_(0, "artdaq::database::configuration::CreateConfigsOperationDetail"
                << "trace_enable");
}
