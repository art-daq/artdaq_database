#include "artdaq-database/ConfigurationDB/common.h"

#include "artdaq-database/ConfigurationDB/dboperation_managedocument.h"

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

#define TRACE_NAME "detail_managedocument.cpp"

namespace db = artdaq::database;
namespace cf = db::configuration;
namespace cftd = cf::debug::detail;
namespace jsonliteral = db::dataformats::literal;

using cf::ManageDocumentOperation;
using cf::options::data_format_t;

using Options = cf::ManageDocumentOperation;

using artdaq::database::basictypes::FhiclData;
using artdaq::database::basictypes::JsonData;
using artdaq::database::basictypes::XmlData;
using artdaq::database::docrecord::JSONDocument;
using artdaq::database::docrecord::JSONDocumentBuilder;

namespace artdaq {
namespace database {
bool json_db_to_gui(std::string const&, std::string&);
bool json_gui_to_db(std::string const&, std::string&);
}  // namespace database
}  // namespace artdaq

namespace artdaq {
namespace database {
namespace fhicljson {
  bool fhicl_to_ast(std::string const& fcl, std::string const& filename, jsn::object_t& json);
  bool fhicl_to_json(std::string const& fcl, std::string const& filename, std::string& json);

}  // namespace fhicljson  
namespace configuration {
namespace detail {
using provider_write_t = void (*)(const Options&, const JSONDocument&);
using provider_read_t = JSONDocument (*)(const Options&, const JSONDocument&);

using provider_call_t = std::vector<JSONDocument> (*)(const Options&, const JSONDocument&);

void write_document(Options const& options, std::string& conf) {
  confirm(options.operation() == apiliteral::operation::writedocument ||
          options.operation() == apiliteral::operation::overwritedocument);
  
  TLOG(20) << "write_document: begin";

  if (conf.empty()) {
    TLOG(21) << "Error in write_document: Invalid configuration document; configuration document is empty.";

    throw runtime_error("write_document") << "Invalid configuration document; configuration document is empty.";
  }

  validate_dbprovider_name(options.provider());

  auto data =  JsonData{conf};
  auto returnValue = std::string{};
  auto returnValueChanged = bool{false};

  switch (options.format()) {
    default:
    case data_format_t::unknown: {
      throw runtime_error("write_document")
          << "Invalid data format; data format=" << cf::to_string(options.format()) << ".";
      break;
    }
    case data_format_t::json: {
      data = JsonData{std::string("{\"data\":").append(conf).append("}")};
      break;
    }
    case data_format_t::db: {
      data = JsonData{conf};
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
      data =  FhiclData{conf};

#if 0      
      auto fhicl = FhiclData{};

      if (!data.convert_to<FhiclData>(fhicl)) {
        TLOG(22) << "write_document: Unable to convert json data to fcl; json=<" << data << ">";

        throw runtime_error("write_document") << "Unable to reverse fhicl-to-json convertion";
      } else {
        TLOG(23) << "write_document: Converted json data to fcl; json=<" << data << ">";
        TLOG(24) << "write_document: Converted json data to fcl; fcl=<" << fhicl << ">";
      }

      returnValue = fhicl;
      returnValueChanged = true;
#endif
      break;
    }
    case data_format_t::xml: {
      // convert from xml to json and back to xml

      data = XmlData{conf};
#if 0
      auto xml = XmlData{};

      if (!data.convert_to<XmlData>(xml)) {
        TLOG(25) << "write_document: Unable to convert json data to fcl; json=<" << data << ">";

        throw runtime_error("write_document") << "Unable to reverse xml-to-json convertion";
      } else {
        TLOG(26) << "write_document: Converted json data to fcl; json=<" << data << ">";
        TLOG(27) << "write_document: Converted json data to fcl; fcl=<" << xml << ">";
      }

      returnValue = xml;
      returnValueChanged = true;
#endif
      break;
    }
  }

  //TLOG(28) << "write_document: json_buffer=<" << data << ">";
  TLOG(29) << "write_document: options=<" << options << ">";

  // create a json document to be inserted into the database
  JSONDocumentBuilder builder{{data}};

  auto filter = std::string{", \"filter\":"};
  filter.append(options.query_filter_to_JsonData());

  if (options.format() != data_format_t::db && options.format() != data_format_t::gui) {
    builder.createFromData(data);
  } else if (options.format() == data_format_t::gui) {
    builder.newObjectID();
    builder.removeAllConfigurations();
    builder.removeAllEntities();

    filter = std::string{", \"filter\":"} + builder.getObjectID().to_string();
  } else {
    if (builder.isReadonlyOrDeleted() && options.operation() == apiliteral::operation::writedocument) {
      builder.newObjectID();
    }

    filter = std::string{", \"filter\":"} + builder.getObjectID().to_string();
  }

  if (!builder.isReadonlyOrDeleted()) {
    //      if(options.configuration()!=jsonliteral::notprovided)
    builder.addConfiguration({options.configuration_to_JsonData()});

    //      if(options.version()!=jsonliteral::notprovided)
    builder.setVersion({options.version_to_JsonData()});

    //      if(options.collection()!=jsonliteral::notprovided)
    builder.setCollection({options.collection_to_JsonData()});

    //      if(options.entity()!=jsonliteral::notprovided)
    builder.addEntity({options.entity_to_JsonData()});

    if (options.run() != jsonliteral::notprovided) {
      builder.addRun({options.run_to_JsonData()});
    }
  }

  auto insert_payload =
      JsonData{"{\"document\":" + builder.to_string() + filter + ", \"collection\":\"" + options.collection() + "\"}"};

  TLOG(30) << "write_document: insert_payload=<" << insert_payload << ">";
  
  auto dispatch_persistence_provider = [](std::string const& name) {
    auto providers =
        std::map<std::string, provider_write_t>{{apiliteral::provider::mongo, cf::mongo::writeDocument},
                                                {apiliteral::provider::filesystem, cf::filesystem::writeDocument},
                                                {apiliteral::provider::ucon, cf::ucon::writeDocument}};

    return providers.at(name);
  };

  dispatch_persistence_provider(options.provider())(options, insert_payload);

  if (returnValueChanged) {
    conf.swap(returnValue);
  }

  TLOG(31) << "write_document: end";
}

void read_document(Options const& options, std::string& conf) {
  confirm(options.operation() == apiliteral::operation::readdocument);

  TLOG(32) << "read_document: begin";

  if (!conf.empty()) {
    throw runtime_error("read_document") << "Invalid configuration document; configuration document is not empty.";
  }

  validate_dbprovider_name(options.provider());

  auto search_payload = JsonData{"{\"filter\":" + options.query_filter_to_JsonData().json_buffer +
                                 +", \"collection\":\"" + options.collection() + "\"}"};

  TLOG(33) << "read_document: search_payload=<" << search_payload << ">";

  auto dispatch_persistence_provider = [](std::string const& name) {
    auto providers =
        std::map<std::string, provider_read_t>{{apiliteral::provider::mongo, cf::mongo::readDocument},
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

    if (!jsn::JsonReader{}.read(search_result, resultAst)) {
      throw runtime_error("read_document") << "Invalid json data";
    }

    auto const& docAst = boost::get<jsn::object_t>(resultAst.at(jsonliteral::origin));
    format = to_data_format(boost::get<std::string>(docAst.at(jsonliteral::format)));

    TLOG(34) << "read_document: format=<" << to_string(format) << ">";
  }

  switch (format) {
    default:
    case data_format_t::db: {
      returnValue = search_result;
      returnValueChanged = true;
      break;
    }

    case data_format_t::gui: {
      if (!db::json_db_to_gui(search_result, returnValue)) {
        throw runtime_error("read_document") << "DB to GUI data convertion failed";
      }

      returnValueChanged = true;
      break;
    }

    case data_format_t::json: {
      auto resultAst = jsn::object_t{};

      if (!jsn::JsonReader{}.read(search_result, resultAst)) {
        throw runtime_error("read_document") << "Invalid json data";
      }

      auto const& docAst = boost::get<jsn::object_t>(resultAst.at(jsonliteral::document));
      auto const& dataAst = boost::get<jsn::object_t>(docAst.at(jsonliteral::data));

      if (!jsn::JsonWriter{}.write(dataAst, returnValue)) {
        throw runtime_error("read_document") << "Invalid json data";
      }

      returnValueChanged = true;
      break;
    }
    case data_format_t::unknown:
      throw runtime_error("read_document")
          << "Invalid data format; data format=" << cf::to_string(options.format()) << ".";
      break;

    case data_format_t::fhicl: {
      auto document = JSONDocument{search_result};
      auto json = JsonData{document.findChild(jsonliteral::document).to_string()};

      auto fhicl = FhiclData{};
      if (!json.convert_to<FhiclData>(fhicl)) {
        TLOG(35) << "read_document: Unable to convert json data to fcl; json=<" << json << ">";

        throw runtime_error("read_document") << "Unable to reverse fhicl-to-json convertion";
      }
      returnValue = fhicl.fhicl_buffer;

      returnValueChanged = true;

      break;
    }
    case data_format_t::xml: {
      auto document = JSONDocument{search_result};
      auto json = JsonData{document.findChild(jsonliteral::document).to_string()};

      auto xml = XmlData{};
      if (!json.convert_to<XmlData>(xml)) {
        TLOG(36) << "read_document: Unable to convert json data to xml; json=<" << json << ">";

        throw runtime_error("read_document") << "Unable to reverse xml-to-json convertion";
      }
      returnValue = xml.xml_buffer;

      returnValueChanged = true;

      break;
    }
  }

  if (returnValueChanged) {
    conf.swap(returnValue);
  }

  TLOG(18) << "read_document: end";
}

void find_versions(Options const& options, std::string& versions) {
  confirm(versions.empty());
  confirm(options.operation() == apiliteral::operation::findversions);

  TLOG(38) << "find_versions: begin";
  TLOG(39) << "find_versions args options=<" << options << ">";

  validate_dbprovider_name(options.provider());

  auto dispatch_persistence_provider = [](std::string const& name) {
    auto providers =
        std::map<std::string, provider_call_t>{{apiliteral::provider::mongo, cf::mongo::findVersions},
                                               {apiliteral::provider::filesystem, cf::filesystem::findVersions},
                                               {apiliteral::provider::ucon, cf::ucon::findVersions}};

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
    case data_format_t::fhicl: {
      throw runtime_error("find_versions") << "Unsupported data format.";
      break;
    }
   case data_format_t::gui: {
      std::ostringstream oss;      
      oss << "{ \"search\": [\n";
      for (auto const& search_result : search_results){
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
      for (auto const& search_result : search_results){
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
    versions.swap(returnValue);
  }

  TLOG(40) << "find_versions: end";
}

void find_entities(Options const& options, std::string& entities) {
  confirm(entities.empty());
  confirm(options.operation() == apiliteral::operation::findentities);

  TLOG(41) << "find_entities: begin";
  TLOG(42) << "find_entities args options=<" << options << ">";

  validate_dbprovider_name(options.provider());

  auto dispatch_persistence_provider = [](std::string const& name) {
    auto providers =
        std::map<std::string, provider_call_t>{{apiliteral::provider::mongo, cf::mongo::findEntities},
                                               {apiliteral::provider::filesystem, cf::filesystem::findEntities},
                                               {apiliteral::provider::ucon, cf::ucon::findEntities}};

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
    case data_format_t::fhicl: {
      throw runtime_error("find_entities") << "Unsupported data format.";
      break;
    }

   case data_format_t::gui: {
      std::ostringstream oss;      
      oss << "{ \"search\": [\n";
      for (auto const& search_result : search_results){
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
      for (auto const& search_result : search_results){
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
    entities.swap(returnValue);
  }

  TLOG(43) << "find_entities: end";
  TLOG(44) << "find_entities: end entities=" << entities;
}

void add_entity(Options const& options, std::string& conf) {
  confirm(conf.empty());
  confirm(options.operation() == apiliteral::operation::addentity);

  TLOG(45) << "add_entity: begin";
  TLOG(46) << "add_entity args options=<" << options << ">";
  TLOG(47) << "add_entity args conf=<" << conf << ">";

  validate_dbprovider_name(options.provider());

  auto document = std::string{};

  Options newOptions = options;
  newOptions.operation(apiliteral::operation::readdocument);
  newOptions.format(data_format_t::db);
  newOptions.entity(apiliteral::notprovided);

  newOptions.queryFilter(apiliteral::notprovided);

  read_document(newOptions, document);
  JSONDocumentBuilder builder{{document}};

  builder.addEntity({options.entity_to_JsonData()});

  newOptions.operation(apiliteral::operation::writedocument);
  newOptions.format(data_format_t::db);

  auto updated = builder.to_string();
  write_document(newOptions, updated);
  newOptions.operation(apiliteral::operation::readdocument);
  newOptions.format(options.format());

  document.clear();
  read_document(newOptions, document);

  conf.swap(document);

  TLOG(48) << "add_entity end conf=<" << conf << ">";
}

void remove_entity(Options const& options, std::string& conf) {
  confirm(conf.empty());
  confirm(options.operation() == apiliteral::operation::rmentity);

  TLOG(49) << "remove_entity: begin";
  TLOG(50) << "remove_entity args options=<" << options << ">";
  TLOG(51) << "remove_entity args conf=<" << conf << ">";

  validate_dbprovider_name(options.provider());

  auto document = std::string{};

  Options newOptions = options;
  newOptions.operation(apiliteral::operation::readdocument);
  newOptions.format(data_format_t::db);
  newOptions.entity(apiliteral::notprovided);
  newOptions.queryFilter(apiliteral::notprovided);

  read_document(newOptions, document);
  JSONDocumentBuilder builder{{document}};

  builder.removeEntity({options.entity_to_JsonData()});

  newOptions.operation(apiliteral::operation::writedocument);
  newOptions.format(data_format_t::db);

  auto updated = builder.to_string();
  write_document(newOptions, updated);

  newOptions.operation(apiliteral::operation::readdocument);
  newOptions.format(options.format());

  document.clear();
  read_document(newOptions, document);

  conf.swap(document);

  TLOG(52) << "remove_entity end conf=<" << conf << ">";
}

void mark_document_readonly(Options const& options, std::string& conf) {
  confirm(conf.empty());
  confirm(options.operation() == apiliteral::operation::markreadonly);

  TLOG(53) << "mark_document_readonly: begin";
  TLOG(54) << "mark_document_readonly args options=<" << options << ">";
  TLOG(55) << "mark_document_readonly args conf=<" << conf << ">";

  validate_dbprovider_name(options.provider());

  auto document = std::string{};

  Options newOptions = options;
  newOptions.operation(apiliteral::operation::readdocument);
  newOptions.format(data_format_t::db);
  newOptions.entity(apiliteral::notprovided);
  newOptions.queryFilter(apiliteral::notprovided);

  read_document(newOptions, document);
  JSONDocumentBuilder builder{{document}};

  builder.markReadonly();

  newOptions.operation(apiliteral::operation::overwritedocument);
  newOptions.format(data_format_t::db);

  auto updated = builder.to_string();
  write_document(newOptions, updated);

  newOptions.operation(apiliteral::operation::readdocument);
  newOptions.format(options.format());

  document.clear();
  read_document(newOptions, document);

  conf.swap(document);

  TLOG(56) << "mark_document_readonly end conf=<" << conf << ">";
}

void mark_document_deleted(Options const& options, std::string& conf) {
  confirm(conf.empty());
  confirm(options.operation() == apiliteral::operation::markdeleted);

  TLOG(57) << "mark_document_deleted: begin";
  TLOG(58) << "mark_document_deleted args options=<" << options << ">";
  TLOG(59) << "mark_document_deleted args conf=<" << conf << ">";

  validate_dbprovider_name(options.provider());

  auto document = std::string{};

  Options newOptions = options;
  newOptions.operation(apiliteral::operation::readdocument);
  newOptions.format(data_format_t::db);
  newOptions.entity(apiliteral::notprovided);
  newOptions.queryFilter(apiliteral::notprovided);

  read_document(newOptions, document);
  JSONDocumentBuilder builder{{document}};

  builder.markDeleted();

  newOptions.operation(apiliteral::operation::overwritedocument);
  newOptions.format(data_format_t::db);

  auto updated = builder.to_string();
  write_document(newOptions, updated);

  newOptions.operation(apiliteral::operation::readdocument);
  newOptions.format(options.format());

  document.clear();
  read_document(newOptions, document);

  conf.swap(document);

  TLOG(60) << "mark_document_deleted end conf=<" << conf << ">";
}

void read_documents(ManageDocumentOperation const& options, std::vector<JSONDocument>& document_list) {
  confirm(document_list.empty());
  confirm(options.operation() == apiliteral::operation::readdocument);

  using provider_call_t = std::vector<JSONDocument> (*)(const ManageDocumentOperation&, const JSONDocument&);

  TLOG(61) << "read_documents: begin";

  validate_dbprovider_name(options.provider());

  std::ostringstream oss;
  oss << "{" << quoted_(jsonliteral::filter) << ":" << options.query_filter_to_JsonData() << ",";
  oss << quoted_(jsonliteral::collection) << ":" << quoted_(options.collection()) << "}";

  auto search_payload = oss.str();

  TLOG(62) << "read_documents: search_payload=<" << search_payload << ">";

  auto dispatch_persistence_provider = [](std::string const& name) {
    auto providers =
        std::map<std::string, provider_call_t>{{apiliteral::provider::mongo, cf::mongo::readDocuments},
                                               {apiliteral::provider::filesystem, cf::filesystem::readDocuments},
                                               {apiliteral::provider::ucon, cf::ucon::readDocuments}};
    return providers.at(name);
  };

  auto documents = dispatch_persistence_provider(options.provider())(options, search_payload);
  document_list.swap(documents);

  TLOG(11) << "read_documents: found " << document_list.size() << "documents.";
  TLOG(12) << "read_documents: end";
}

void write_documents(ManageDocumentOperation const& options, std::vector<JSONDocument> const& document_list) {
  confirm(!document_list.empty());
  confirm(options.operation() == apiliteral::operation::writedocument);

  TLOG(13) << "write_documents: begin";

  auto dispatch_persistence_provider = [](std::string const& name) {
    auto providers =
        std::map<std::string, provider_write_t>{{apiliteral::provider::mongo, cf::mongo::writeDocument},
                                                {apiliteral::provider::filesystem, cf::filesystem::writeDocument},
                                                {apiliteral::provider::ucon, cf::ucon::writeDocument}};
    return providers.at(name);
  };

  TLOG(14) << "write_documents: writing " << document_list.size() << " documents.";

  for (auto const& document : document_list) {
    JSONDocumentBuilder builder{document};

    if (builder.isReadonlyOrDeleted()) {
      builder.newObjectID();
    }

    std::ostringstream oss;
    oss << "{" << quoted_(jsonliteral::document) << ":" << document << ",";
    oss << quoted_(jsonliteral::filter) << ":" << builder.getObjectID().to_string() << ",";
    oss << quoted_(jsonliteral::collection) << ":" << quoted_(options.collection()) << "}";

    dispatch_persistence_provider(options.provider())(options, {oss.str()});

    TLOG(15) << "write_documents: wrote document ouid=" << builder.getObjectOUID();
  }

  TLOG(16) << "write_documents: end";
}
}  // namespace detail
}  // namespace configuration
}  // namespace database
}  // namespace artdaq

void cftd::ManageDocuments() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);

  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TLOG(17) << "artdaq::database::configuration::ManageDocuments trace_enable";
}
