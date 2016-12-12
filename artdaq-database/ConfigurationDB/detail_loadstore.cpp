#include "artdaq-database/ConfigurationDB/common.h"

#include "artdaq-database/ConfigurationDB/dboperation_loadstore.h"

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

#define TRACE_NAME "CONF:LdStrD_C"

namespace db = artdaq::database;
namespace cf = db::configuration;
namespace cfl = cf::literal;
namespace cflo = cfl::operation;
namespace cflp = cfl::provider;
namespace cfld = cfl::document;
namespace cfls = cfl::origin;

namespace cftd = cf::debug::detail;

using cf::LoadStoreOperation;
using cf::options::data_format_t;

using Options = cf::LoadStoreOperation;

using artdaq::database::basictypes::JsonData;
using artdaq::database::basictypes::FhiclData;
using artdaq::database::basictypes::XmlData;
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
typedef JsonData (*provider_load_t)(Options const& /*options*/, JsonData const& /*search_payload*/);
typedef void (*provider_store_t)(Options const& /*options*/, JsonData const& /*insert_payload*/);

void store_configuration(Options const& options, std::string& conf) {
  assert(!conf.empty());
  assert(options.operation().compare(cflo::store) == 0);

  TRACE_(15, "store_configuration: begin");

  if (conf.empty()) {
    TRACE_(15,
           "Error in store_configuration: Invalid configuration; "
           "configuration is empty.");

    throw cet::exception("store_configuration") << "Invalid configuration; configuration is empty.";
  }

  validate_dbprovider_name(options.provider());

  auto data = JsonData{conf};
  auto returnValue = std::string{};
  auto returnValueChanged = bool{false};

  switch (options.dataFormat()) {
    default:
    case data_format_t::json:
      data = JsonData{std::string("{\"data\":").append(conf).append("}")};
      break;

    case data_format_t::unknown: {
      throw cet::exception("store_configuration")
          << "Invalid data format; data format=" << cf::to_string(options.dataFormat()) << ".";
      break;
    }
    case data_format_t::gui: {
      if (!db::json_gui_to_db(conf, returnValue)) {
        throw cet::exception("store_configuration") << "GUI to DB data convertion failed";
      }

      data = JsonData{returnValue};

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

      returnValue = fhicl.fhicl_buffer;
      returnValueChanged = true;

      break;
    }
    case data_format_t::xml: {
      // convert from xml to json and back to xml

      data = XmlData{conf};

      auto xml = XmlData{};

      if (!data.convert_to<XmlData>(xml)) {
        TRACE_(17, "store_configuration: Unable to convert json data to fcl; json=<" << data.json_buffer << ">");

        throw cet::exception("store_configuration") << "Unable to reverse xml-to-json convertion";
      } else {
        TRACE_(17, "store_configuration: Converted json data to fcl; json=<" << data.json_buffer << ">");
        TRACE_(17, "store_configuration: Converted json data to fcl; fcl=<" << xml.xml_buffer << ">");
      }

      returnValue = xml.xml_buffer;
      returnValueChanged = true;

      break;
    }
  }

  TRACE_(15, "store_configuration: json_buffer=<" << data.json_buffer << ">");

  // create a json document to be inserted into the database
  auto builder = JSONDocumentBuilder{};

  builder.createFromData(data.json_buffer);

  auto globalConfiguration = JSONDocument{options.globalConfiguration_to_JsonData().json_buffer};
  auto version = JSONDocument{options.version_to_JsonData().json_buffer};
  auto configurableEntity = JSONDocument{options.configurableEntity_to_JsonData().json_buffer};

  builder.addToGlobalConfig(globalConfiguration);
  builder.setVersion(version);
  builder.setConfigurableEntity(configurableEntity);

  auto document = std::move(builder.extract());
  auto insert_payload =
      JsonData{"{\"document\":" + document.to_string() + ", \"collection\":\"" + options.collectionName() + "\"}"};

  TRACE_(15, "store_configuration: insert_payload=<" << insert_payload.json_buffer << ">");

  auto dispatch_persistence_provider = [](std::string const& name) {
    auto providers = std::map<std::string, provider_store_t>{
        {cflp::mongo, cf::mongo::store}, {cflp::filesystem, cf::filesystem::store}, {cflp::ucond, cf::ucond::store}};

    return providers.at(name);
  };

  dispatch_persistence_provider(options.provider())(options, insert_payload);

  if (returnValueChanged) conf.swap(returnValue);

  TRACE_(15, "store_configuration: end");
}

void load_configuration(Options const& options, std::string& conf) {
  assert(conf.empty());
  assert(options.operation().compare(cflo::load) == 0);

  TRACE_(16, "load_configuration: begin");

  if (!conf.empty()) {
    throw cet::exception("load_configuration") << "Invalid configuration; configuration is not empty.";
  }

  validate_dbprovider_name(options.provider());

  auto search_payload = JsonData{"{\"filter\":" + options.search_filter_to_JsonData().json_buffer +
                                 +", \"collection\":\"" + options.collectionName() + "\"}"};

  TRACE_(16, "load_configuration: search_payload=<" << search_payload.json_buffer << ">");

  auto dispatch_persistence_provider = [](std::string const& name) {
    auto providers = std::map<std::string, provider_load_t>{
        {cflp::mongo, cf::mongo::load}, {cflp::filesystem, cf::filesystem::load}, {cflp::ucond, cf::ucond::load}};

    return providers.at(name);
  };

  auto search_result = dispatch_persistence_provider(options.provider())(options, search_payload);

  auto returnValue = std::string{};
  auto returnValueChanged = bool{false};

  auto dataFormat = options.dataFormat();

  if (dataFormat == data_format_t::origin) {
    auto resultAst = jsn::object_t{};

    if (!jsn::JsonReader{}.read(search_result.json_buffer, resultAst))
      throw cet::exception("load_configuration") << "Invalid json data";

    auto const& docAst = boost::get<jsn::object_t>(resultAst.at(cfls::origin));
    dataFormat = to_data_format(boost::get<std::string>(docAst.at(cfls::format)));

    TRACE_(16, "load_configuration: dataFormat=<" << to_string(dataFormat) << ">");
  }

  switch (dataFormat) {
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
      auto resultAst = jsn::object_t{};

      if (!jsn::JsonReader{}.read(search_result.json_buffer, resultAst))
        throw cet::exception("load_configuration") << "Invalid json data";

      auto const& docAst = boost::get<jsn::object_t>(resultAst.at(cfld::document));
      auto const& dataAst = boost::get<jsn::object_t>(docAst.at(cfld::data));

      if (!jsn::JsonWriter{}.write(dataAst, returnValue))
        throw cet::exception("load_configuration") << "Invalid json data";

      returnValueChanged = true;
      break;
    }
    case data_format_t::unknown:
      throw cet::exception("load_configuration")
          << "Invalid data format; data format=" << cf::to_string(options.dataFormat()) << ".";
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
    case data_format_t::xml: {
      auto document = JSONDocument{search_result.json_buffer};
      auto json = JsonData{document.findChild(db::jsonutils::literal::document).to_string()};

      auto xml = XmlData{};
      if (!json.convert_to<XmlData>(xml)) {
        TRACE_(16, "load_configuration: Unable to convert json data to xml; json=<" << json.json_buffer << ">");

        throw cet::exception("load_configuration") << "Unable to reverse xml-to-json convertion";
      }
      returnValue = xml.xml_buffer;

      returnValueChanged = true;

      break;
    }
  }

  if (returnValueChanged) conf.swap(returnValue);

  TRACE_(15, "load_configuration: end");
}
}  // namespace detail
}  // namespace configuration
}  // namespace database
}  // namespace artdaq

void cftd::enableLoadStoreOperation() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);

  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TRACE_(0, "artdaq::database::configuration::LoadStoreOperationDetail trace_enable");
}
