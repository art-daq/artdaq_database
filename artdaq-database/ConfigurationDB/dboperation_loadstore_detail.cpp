#include "artdaq-database/ConfigurationDB/common.h"

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

#define TRACE_NAME "CONF:OpLdStrD_C"

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
typedef JsonData (*provider_load_t)(Options const& /*options*/, JsonData const& /*search_payload*/);
typedef void (*provider_store_t)(Options const& /*options*/, JsonData const& /*insert_payload*/);

void store_configuration(Options const& options, std::string& conf) {
  assert(!conf.empty());
  assert(options.operation().compare(cfo::literal::operation_store) == 0);

  TRACE_(15, "store_configuration: begin");

  if (conf.empty()) {
    TRACE_(15,
           "Error in store_configuration: Invalid configuration; "
           "configuration is empty.");

    throw cet::exception("store_configuration") << "Invalid configuration; configuration is empty.";
  }

  if (cf::not_equal(options.provider(), cfol::database_provider_filesystem) &&
      cf::not_equal(options.provider(), cfol::database_provider_mongo)) {
    TRACE_(15,
           "Error in store_configuration: Invalid database provider; "
           "database provider="
               << options.provider() << ".");

    throw cet::exception("store_configuration") << "Invalid database provider; database provider=" << options.provider()
                                                << ".";
  }

  auto data = JsonData{conf};
  auto returnValue = std::string{};
  auto returnValueChanged = bool{false};

  switch (options.dataFormat()) {
    default:
    case data_format_t::json:
      // do nothing
      break;

    case data_format_t::unknown: {
      throw cet::exception("store_configuration")
          << "Invalid data format; data format=" << cf::decode(options.dataFormat()) << ".";
      break;
    }
    case data_format_t::gui: {
      if (!db::json_gui_to_db(conf, returnValue)) {
        throw cet::exception("load_configuration") << "GUI to DB data convertion failed";
      }
      returnValueChanged = true;

      break;
    }
    case data_format_t::fhicl: {
      // convert from fhicl to json and back to fhicl
      data = FhiclData{conf};

      auto fhicl = FhiclData{};

      if (!data.convert_to<FhiclData>(fhicl)) {
        TRACE_(17, "store_configuration: Unable to convert json data to fcl; json=<" << data.json_buffer << ">");

        throw cet::exception("store_configuration") << "Unable to reverse fhicl-to-json convertion";
      } else {
        TRACE_(17, "store_configuration: Converted json data to fcl; json=<" << data.json_buffer << ">");
        TRACE_(17, "store_configuration: Converted json data to fcl; fcl=<" << fhicl.fhicl_buffer << ">");
      }

      returnValueChanged = true;

      break;
    }
  }

  TRACE_(15, "store_configuration: json_buffer=<" << data.json_buffer << ">");

  // create a json document to be inserted into the database
  auto builder = JSONDocumentBuilder{};
  builder.createFromData(data.json_buffer);
  builder.addToGlobalConfig(options.globalConfigurationId_jsndoc());
  builder.setVersion(options.version_jsndoc());
  builder.setConfigurableEntity(options.configurableEntity_jsndoc());

  auto document = std::move(builder.extract());
  auto insert_payload =
      JsonData{"{\"document\":" + document.to_string() + ", \"collection\":\"" + options.type() + "\"}"};

  TRACE_(15, "store_configuration: insert_payload=<" << insert_payload.json_buffer << ">");

  auto dispatch_persistence_provider = [](std::string const& name) {
    auto providers = std::map<std::string, provider_store_t>{
        {cfol::database_provider_mongo, cf::mongo::store}, {cfol::database_provider_filesystem, cf::filesystem::store}};

    return providers.at(name);
  };

  dispatch_persistence_provider(options.provider())(options, insert_payload);

  if (returnValueChanged) conf.swap(returnValue);

  TRACE_(15, "store_configuration: end");
}

void load_configuration(Options const& options, std::string& conf) {
  assert(conf.empty());
  assert(options.operation().compare(cfo::literal::operation_load) == 0);

  TRACE_(16, "load_configuration: begin");

  if (!conf.empty()) {
    throw cet::exception("load_configuration") << "Invalid configuration; configuration is not empty.";
  }

  if (cf::not_equal(options.provider(), cfol::database_provider_filesystem) &&
      cf::not_equal(options.provider(), cfol::database_provider_mongo)) {
    TRACE_(16,
           "Error in load_configuration: Invalid database provider; "
           "database provider="
               << options.provider() << ".");

    throw cet::exception("load_configuration") << "Invalid database provider; database provider=" << options.provider()
                                               << ".";
  }

  auto search_payload = JsonData{"{\"filter\":" + options.search_filter_jsondoc().to_string() + +", \"collection\":\"" +
                                 options.type() + "\"}"};

  TRACE_(16, "load_configuration: search_payload=<" << search_payload.json_buffer << ">");

  auto dispatch_persistence_provider = [](std::string const& name) {
    auto providers = std::map<std::string, provider_load_t>{{cfol::database_provider_mongo, cf::mongo::load},
                                                            {cfol::database_provider_filesystem, cf::filesystem::load}};

    return providers.at(name);
  };

  auto search_result = dispatch_persistence_provider(options.provider())(options, search_payload);

  auto returnValue = std::string{};
  auto returnValueChanged = bool{false};

  switch (options.dataFormat()) {
    default:
    case data_format_t::db: {
      returnValue = search_result.json_buffer;
      returnValueChanged = true;
      break;
    }

    case data_format_t::gui: {
      if (!db::json_db_to_gui(search_result.json_buffer, returnValue)) {
        throw cet::exception("load_configuration") << "DB to GUI data convertion failed";
      }

      returnValueChanged = true;
      break;
    }

    case data_format_t::json: {
      auto document = JSONDocument{search_result.json_buffer};
      returnValue = document.findChild(db::jsonutils::literal::document).to_string();

      returnValueChanged = true;
      break;
    }
    case data_format_t::unknown:
      throw cet::exception("load_configuration")
          << "Invalid data format; data format=" << cf::decode(options.dataFormat()) << ".";
      break;

    case data_format_t::fhicl: {
      auto document = JSONDocument{search_result.json_buffer};
      auto json = JsonData{document.findChild(db::jsonutils::literal::document).to_string()};

      auto fhicl = FhiclData{};
      if (!json.convert_to<FhiclData>(fhicl)) {
        TRACE_(16, "load_configuration: Unable to convert json data to fcl; json=<" << json.json_buffer << ">");

        throw cet::exception("load_configuration") << "Unable to reverse fhicl-to-json convertion";
      }
      returnValue = fhicl.fhicl_buffer;

      returnValueChanged = true;

      break;
    }
  }

  if (returnValueChanged) conf.swap(returnValue);

  TRACE_(15, "load_configuration: end");
}
}

void cf::trace_enable_LoadStoreOperationDetail() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", 1LL);
  TRACE_CNTL("modeS", 1LL);

  TRACE_(0, "artdaq::database::configuration::LoadStoreOperationDetail"
                << "trace_enable");
}
