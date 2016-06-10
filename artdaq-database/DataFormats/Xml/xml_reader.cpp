#include "artdaq-database/DataFormats/common.h"

#include "artdaq-database/DataFormats/Xml/convertxml2json.h"
#include "artdaq-database/DataFormats/Xml/xml_reader.h"
#include "artdaq-database/DataFormats/common/shared_literals.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "XML:XmlReader_C"

namespace fcl = artdaq::database::xml;
namespace jsn = artdaq::database::json;

using artdaq::database::xml::XmlReader;

namespace literal = artdaq::database::dataformats::literal;

bool XmlReader::read(std::string const& in [[gnu::unused]], jsn::object_t& json_object) {
  assert(!in.empty());
  assert(json_object.empty());

  TRACE_(2, "read() begin");

  try {
    auto object = jsn::object_t();
    object[literal::data_node] = jsn::object_t();

    // TODO: convert XML into Json AST

    json_object.swap(object);

    TRACE_(2, "read() end");

    return true;

  } catch (std::exception const& e) {
    TRACE_(2, "read() Caughtexception message=" << e.what());

    std::cerr << "Caught exception message=" << e.what() << "\n";
    throw;
  }
}

void artdaq::database::xml::debug::enableXmlReader() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", 1LL);
  TRACE_CNTL("modeS", 1LL);

  TRACE_(0, "artdaq::database::xml::XmlReader trace_enable");
}
