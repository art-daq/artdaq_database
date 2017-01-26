#include "artdaq-database/ConfigurationDB/common.h"

#include "artdaq-database/ConfigurationDB/dboperation_readwrite.h"

#include "artdaq-database/ConfigurationDB/shared_helper_functions.h"
#include "artdaq-database/DataFormats/shared_literals.h"
#include "artdaq-database/SharedCommon/configuraion_api_literals.h"

#include "artdaq-database/ConfigurationDB/configuration_dbproviders.h"

#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/DataFormats/Json/json_common.h"
#include "artdaq-database/DataFormats/shared_literals.h"

#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"

#include "artdaq-database/ConfigurationDB/options_operations.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "CONF:LdStrD_C"

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
using artdaq::database::basictypes::XmlData;
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
typedef JsonData (*provider_load_t)(Options const& /*options*/, JsonData const& /*query_payload*/);
typedef void (*provider_store_t)(Options const& /*options*/, JsonData const& /*insert_payload*/);

void write_document(Options const& options, std::string& conf) {
  confirm(!conf.empty());
  confirm(options.operation().compare(apiliteral::operation::writedocument) == 0);

  TRACE_(15, "write_document: begin");

  if (conf.empty()) {
    TRACE_(15, "Error in write_document: Invalid configuration document; configuration document is empty.");

    throw runtime_error("write_document") << "Invalid configuration document; configuration document is empty.";
  }

  validate_dbprovider_name(options.provider());

  auto data = JsonData{conf};
  auto returnValue = std::string{};
  auto returnValueChanged = bool{false};

  switch (options.format()) {
    default:
    case data_format_t::json:
      data = JsonData{std::string("{\"data\":").append(conf).append("}")};
      break;

    case data_format_t::unknown: {
      throw runtime_error("write_document") << "Invalid data format; data format=" << cf::to_string(options.format())
                                            << ".";
      break;
    }
    case data_format_t::gui: {
      if (!db::json_gui_to_db(conf, returnValue)) {
        throw runtime_error("write_document") << "GUI to DB data convertion failed";
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
        TRACE_(17, "write_document: Unable to convert json data to fcl; json=<" << data.json_buffer << ">");

        throw runtime_error("write_document") << "Unable to reverse fhicl-to-json convertion";
      } else {
        TRACE_(17, "write_document: Converted json data to fcl; json=<" << data.json_buffer << ">");
        TRACE_(17, "write_document: Converted json data to fcl; fcl=<" << fhicl.fhicl_buffer << ">");
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
        TRACE_(17, "write_document: Unable to convert json data to fcl; json=<" << data.json_buffer << ">");

        throw runtime_error("write_document") << "Unable to reverse xml-to-json convertion";
      } else {
        TRACE_(17, "write_document: Converted json data to fcl; json=<" << data.json_buffer << ">");
        TRACE_(17, "write_document: Converted json data to fcl; fcl=<" << xml.xml_buffer << ">");
      }

      returnValue = xml.xml_buffer;
      returnValueChanged = true;

      break;
    }
  }

  TRACE_(15, "write_document: json_buffer=<" << data.json_buffer << ">");
  TRACE_(15, "write_document: options=<" << options.to_string() << ">");

  // create a json document to be inserted into the database
  JSONDocumentBuilder builder{};

  builder.createFromData(data.json_buffer);

  auto configuration = JSONDocument{options.configuration_to_JsonData().json_buffer};
  auto version = JSONDocument{options.version_to_JsonData().json_buffer};
  auto entity = JSONDocument{options.entity_to_JsonData().json_buffer};
  auto collection = JSONDocument{options.collection_to_JsonData().json_buffer};
  
  builder.addConfiguration(configuration);
  builder.setVersion(version);
  builder.setCollection(collection);
  builder.addEntity(entity);

  auto insert_payload =
      JsonData{"{\"document\":" + builder.to_string() + ", \"collection\":\"" + options.collection() + "\"}"};

  TRACE_(15, "write_document: insert_payload=<" << insert_payload.json_buffer << ">");

  auto dispatch_persistence_provider = [](std::string const& name) {
    auto providers = std::map<std::string, provider_store_t>{{apiliteral::provider::mongo, cf::mongo::writeDocument},
                                                             {apiliteral::provider::filesystem, cf::filesystem::writeDocument},
                                                             {apiliteral::provider::ucon, cf::ucon::writeDocument}};

    return providers.at(name);
  };

  dispatch_persistence_provider(options.provider())(options, insert_payload);

  if (returnValueChanged) conf.swap(returnValue);

  TRACE_(15, "write_document: end");
}

void read_document(Options const& options, std::string& conf) {
  confirm(conf.empty());
  confirm(options.operation().compare(apiliteral::operation::readdocument) == 0);

  TRACE_(16, "read_document: begin");

  if (!conf.empty()) {
    throw runtime_error("read_document") << "Invalid configuration document; configuration document is not empty.";
  }

  validate_dbprovider_name(options.provider());

  auto search_payload = JsonData{"{\"filter\":" + options.query_filter_to_JsonData().json_buffer +
                                 +", \"collection\":\"" + options.collection() + "\"}"};

  TRACE_(16, "read_document: search_payload=<" << search_payload.json_buffer << ">");

  auto dispatch_persistence_provider = [](std::string const& name) {
    auto providers = std::map<std::string, provider_load_t>{{apiliteral::provider::mongo, cf::mongo::readDocument},
                                                            {apiliteral::provider::filesystem, cf::filesystem::readDocument},
                                                            {apiliteral::provider::ucon, cf::ucon::readDocument}};

    return providers.at(name);
  };

  auto search_result = dispatch_persistence_provider(options.provider())(options, search_payload);

  auto returnValue = std::string{};
  auto returnValueChanged = bool{false};

  auto format = options.format();

  if (format == data_format_t::origin) {
    auto resultAst = jsn::object_t{};

    if (!jsn::JsonReader{}.read(search_result.json_buffer, resultAst))
      throw runtime_error("read_document") << "Invalid json data";

    auto const& docAst = boost::get<jsn::object_t>(resultAst.at(jsonliteral::origin));
    format = to_data_format(boost::get<std::string>(docAst.at(jsonliteral::format)));

    TRACE_(16, "read_document: format=<" << to_string(format) << ">");
  }

  switch (format) {
    default:
    case data_format_t::db: {
      returnValue = search_result.json_buffer;
      returnValueChanged = true;
      break;
    }

    case data_format_t::gui: {
      if (!db::json_db_to_gui(search_result.json_buffer, returnValue)) {
        throw runtime_error("read_document") << "DB to GUI data convertion failed";
      }

      returnValueChanged = true;
      break;
    }

    case data_format_t::json: {
      auto resultAst = jsn::object_t{};

      if (!jsn::JsonReader{}.read(search_result.json_buffer, resultAst))
        throw runtime_error("read_document") << "Invalid json data";

      auto const& docAst = boost::get<jsn::object_t>(resultAst.at(jsonliteral::document));
      auto const& dataAst = boost::get<jsn::object_t>(docAst.at(jsonliteral::data));

      if (!jsn::JsonWriter{}.write(dataAst, returnValue)) throw runtime_error("read_document") << "Invalid json data";

      returnValueChanged = true;
      break;
    }
    case data_format_t::unknown:
      throw runtime_error("read_document") << "Invalid data format; data format=" << cf::to_string(options.format())
                                           << ".";
      break;

    case data_format_t::fhicl: {
      auto document = JSONDocument{search_result.json_buffer};
      auto json = JsonData{document.findChild(jsonliteral::document).to_string()};

      auto fhicl = FhiclData{};
      if (!json.convert_to<FhiclData>(fhicl)) {
        TRACE_(16, "read_document: Unable to convert json data to fcl; json=<" << json.json_buffer << ">");

        throw runtime_error("read_document") << "Unable to reverse fhicl-to-json convertion";
      }
      returnValue = fhicl.fhicl_buffer;

      returnValueChanged = true;

      break;
    }
    case data_format_t::xml: {
      auto document = JSONDocument{search_result.json_buffer};
      auto json = JsonData{document.findChild(jsonliteral::document).to_string()};

      auto xml = XmlData{};
      if (!json.convert_to<XmlData>(xml)) {
        TRACE_(16, "read_document: Unable to convert json data to xml; json=<" << json.json_buffer << ">");

        throw runtime_error("read_document") << "Unable to reverse xml-to-json convertion";
      }
      returnValue = xml.xml_buffer;

      returnValueChanged = true;

      break;
    }
  }

  if (returnValueChanged) conf.swap(returnValue);

  TRACE_(15, "read_document: end");
}
}  // namespace detail
}  // namespace configuration
}  // namespace database
}  // namespace artdaq

void cftd::enableManageDocumentOperation() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);

  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TRACE_(0, "artdaq::database::configuration::ManageDocumentOperationDetail trace_enable");
}
