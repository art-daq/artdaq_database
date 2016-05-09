#include "artdaq-database/DataFormats/common/helper_functions.h"
#include "artdaq-database/DataFormats/common/shared_literals.h"

#include "artdaq-database/DataFormats/Json/json_common.h"
#include "artdaq-database/DataFormats/Xml/xml_common.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "XML:xmljsondb_C"

namespace artdaq {
namespace database {
namespace xmljson {

namespace fcl = artdaq::database::xml;
namespace jsn = artdaq::database::json;

using artdaq::database::json::JsonReader;
using artdaq::database::json::JsonWriter;

using artdaq::database::xml::XmlReader;
using artdaq::database::xml::XmlWriter;

namespace literal = artdaq::database::dataformats::literal;

bool xml_to_json(std::string const& xml, std::string& json) {
  assert(!xml.empty());
  assert(json.empty());

  TRACE_(2, "xml_to_json: begin");

  auto result = bool{false};

  auto json_root = jsn::object_t{};
  json_root[literal::document_node] = jsn::object_t{};
  json_root[literal::origin_node] = jsn::object_t{};

  auto get_object = [&json_root](std::string const& name) -> auto& {
    return boost::get<jsn::object_t>(json_root[name]);
  };

  get_object(literal::origin_node)[literal::source] = std::string("xml_to_json");
  get_object(literal::origin_node)[literal::timestamp] = artdaq::database::dataformats::timestamp();

  auto& json_node = get_object(literal::document_node);

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

bool json_to_xml(std::string const& json, std::string& xml) {
  assert(!json.empty());
  assert(xml.empty());

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

  auto get_object = [&json_root](std::string const& name) -> auto& {
    return boost::get<jsn::object_t>(json_root.at(name));
  };

  auto& json_node = get_object(literal::document_node);

  auto xml1 = std::string{};

  auto writer = XmlWriter{};

  result = writer.write(json_node, xml1);

  if (result) xml.swap(xml1);

  TRACE_(3, "json_to_xml: end");

  return result;
}

void trace_enable_xmljsondb() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", 1LL);
  TRACE_CNTL("modeS", 1LL);

  TRACE_(0, "artdaq::database::fhicljsondb trace_enable");
}
}  // namespace fhicljson
}  // namespace database
}  // namespace artdaq
