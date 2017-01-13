#include "artdaq-database/DataFormats/common.h"

#include "artdaq-database/DataFormats/shared_literals.h"

#include "artdaq-database/DataFormats/Json/json_common.h"
#include "artdaq-database/DataFormats/Xml/xml_common.h"
#include "artdaq-database/DataFormats/Xml/xmljsondb.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "XML:xmljsondb_C"


using artdaq::database::json::JsonReader;
using artdaq::database::json::JsonWriter;

using artdaq::database::xml::XmlReader;
using artdaq::database::xml::XmlWriter;

namespace literal = artdaq::database::dataformats::literal;

namespace dbxj=artdaq::database::xmljson;

namespace xml = artdaq::database::xml;

bool dbxj::xml_to_json(std::string const& xml, std::string& json) {
  confirm(!xml.empty());
  confirm(json.empty());

  TRACE_(2, "xml_to_json: begin");

  auto result = bool{false};

  auto json_root = jsn::object_t{};
  json_root[literal::document] = jsn::object_t{};
  json_root[literal::origin] = jsn::object_t{};

  auto get_object = [&json_root](std::string const& name) -> auto& { return boost::get<jsn::object_t>(json_root[name]); };

  get_object(literal::origin)[literal::format] = std::string("xml");
  get_object(literal::origin)[literal::source] = std::string("xml_to_json");
  get_object(literal::origin)[literal::timestamp] = artdaq::database::timestamp();

  auto& json_node = get_object(literal::document);

  auto reader = xml::XmlReader{};
  result = reader.read(xml, json_node);

  if (!result) return result;

  auto json1 = std::string{};

  auto writer = JsonWriter{};

  result = writer.write(json_root, json1);

  if (result) json.swap(json1);

  TRACE_(2, "xml_to_json: end");

  return result;
}

bool dbxj::json_to_xml(std::string const& json, std::string& xml) {
  confirm(!json.empty());
  confirm(xml.empty());

  TRACE_(3, "json_to_xml: begin");

  auto result = bool{false};

  TRACE_(3, "json_to_xml: Reading JSON buffer..");

  auto json_root = jsn::object_t{};
  auto reader = JsonReader{};
  result = reader.read(json, json_root);

  if (!result) {
    TRACE_(3, "json_to_xml: Unable to read JSON buffer");
    return result;
  }

  auto get_object = [&json_root](std::string const& name) -> auto& { return boost::get<jsn::object_t>(json_root.at(name)); };

  auto& json_node = get_object(literal::document);

  auto xml1 = std::string{};

  auto writer = XmlWriter{};

  result = writer.write(json_node, xml1);

  if (result) xml.swap(xml1);

  TRACE_(3, "json_to_xml: end");

  return result;
}

void dbxj::debug::enableXmlJson() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TRACE_(0, "artdaq::database::xmljson trace_enable");
}
