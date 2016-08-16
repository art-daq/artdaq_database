#include "artdaq-database/DataFormats/common.h"

#include "artdaq-database/DataFormats/Xml/convertxml2json.h"
#include "artdaq-database/DataFormats/Xml/xml_writer.h"
#include "artdaq-database/DataFormats/common/shared_literals.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "XML:XmlWriter_C"

namespace fcl = artdaq::database::xml;
namespace jsn = artdaq::database::json;

using artdaq::database::xml::XmlWriter;

namespace literal = artdaq::database::dataformats::literal;

bool XmlWriter::write(jsn::object_t const& json_object, std::string& out) {
  assert(out.empty());
  assert(!json_object.empty());

  TRACE_(2, "write() begin");

  auto const& data_node = boost::get<jsn::object_t>(json_object.at(literal::data_node));

  auto result = bool(false);
  auto buffer = std::string();

  TRACE_(2, "write() create xml begin");

  for (auto const& data[[gnu::unused]] : data_node) {
    // TODO loop over json AST and convert it to XML
  }

  TRACE_(2, "write() create xml end");

  if (result) out.swap(buffer);

  TRACE_(2, "write() end");

  return result;
}

void artdaq::database::xml::debug::enableXmlWriter() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", 1LL);
  TRACE_CNTL("modeS", 1LL);

  TRACE_(0, "artdaq::database::xml::XmlWriter trace_enable");
}
