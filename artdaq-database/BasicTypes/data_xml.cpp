#include "artdaq-database/BasicTypes/data_json.h"
#include "artdaq-database/BasicTypes/data_json_fusion.h"

#include "artdaq-database/BasicTypes/data_xml.h"
#include "artdaq-database/BasicTypes/data_xml_fusion.h"

#include "artdaq-database/DataFormats/Xml/xml_common.h"
#include "artdaq-database/DataFormats/Xml/xmljsondb.h"

#include "artdaq-database/DataFormats/shared_literals.h"

#include "artdaq-database/BasicTypes/base64.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "BTPS:XmlData_C"

namespace regex {
constexpr auto parse_base64data = "[\\s\\S]*\"base64\"\\s*:\\s*\"(\\S*?)\"";
}

namespace artdaq {
namespace database {
namespace basictypes {

template <>
bool JsonData::convert_to(XmlData& xml) const {
  using artdaq::database::xmljson::json_to_xml;

  return json_to_xml(json_buffer, xml.xml_buffer);
}

template <>
bool JsonData::convert_from(XmlData const& xml) {
  using artdaq::database::xmljson::xml_to_json;

  return xml_to_json(xml.xml_buffer, json_buffer);
}

XmlData::XmlData(std::string const& buffer) : xml_buffer{buffer} {}

XmlData::XmlData(JsonData const& document) {
  namespace literal = artdaq::database::dataformats::literal;

  assert(!document.json_buffer.empty());

  TRACE_(1, "XML document=" << document.json_buffer);

  auto ex = std::regex({regex::parse_base64data});

  auto results = std::smatch();

  if (!std::regex_search(document.json_buffer, results, ex))
    throw std::runtime_error("JSON to XML convertion error, regex_search()==false; JSON buffer: " + document.json_buffer);

  if (results.size() != 1)
    throw std::runtime_error(
        "JSON to XML convertion error, "
        "regex_search().size()!=1; JSON buffer: " +
        document.json_buffer);

  auto base64 = std::string(results[0]);
  TRACE_(2, "XML base64=" << base64);

  auto json = base64_decode(base64);
  TRACE_(3, "XML  json=" << json);

  JsonData(json).convert_to(*this);

  TRACE_(4, "XML xml=" << xml_buffer);
}

XmlData::operator JsonData() const {
  namespace literal = artdaq::database::dataformats::literal;

  TRACE_(5, "XML xml=" << xml_buffer);

  auto json = JsonData("");

  if (!json.convert_from(*this)) throw std::runtime_error("XML to JSON convertion error; XML buffer: " + this->xml_buffer);

  TRACE_(6, "XML  json=" << json.json_buffer);

  auto collection = std::string("XmlData_") + type_version();

  auto base64 = base64_encode(xml_buffer);
  TRACE_(7, "XML base64=" << base64);

  std::ostringstream os;

  os << json.json_buffer;

  TRACE_(8, "XML document=" << os.str());

  return {os.str()};
}

std::istream& operator>>(std::istream& is, XmlData& data) {
  auto str = std::string(std::istreambuf_iterator<char>(is), {});
  auto json = JsonData(str);
  data = XmlData(json);
  return is;
}

std::ostream& operator<<(std::ostream& os, XmlData const& data) {
  os << data.xml_buffer;
  return os;
}

}  // namespace basictypes
}  // namespace database
}  // namespace artdaq
