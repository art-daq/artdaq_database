#include "artdaq-database/DataFormats/common.h"

#include "artdaq-database/DataFormats/Conf/conf_writer.h"
#include "artdaq-database/DataFormats/Conf/convertconf2json.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "CNF:ConfWriter_C"

namespace fcl = artdaq::database::conf;
namespace jsn = artdaq::database::json;

using artdaq::database::conf::ConfWriter;

namespace literal = artdaq::database::dataformats::literal;

bool ConfWriter::write(jsn::object_t const& json_object, std::string& out) {
  confirm(out.empty());
  confirm(!json_object.empty());

  TLOG(12) << "write() begin";

  auto const& data_node = boost::get<jsn::object_t>(json_object.at(literal::data));

  auto result = bool(false);
  auto buffer = std::string();

  TLOG(12) << "write() create conf begin";

  for (auto const& data[[gnu::unused]] : data_node) {
    // TODO loop over json AST and convert it to XML
  }

  TLOG(12) << "write() create conf end";

  if (result) out.swap(buffer);

  TLOG(12) << "write() end";

  return result;
}

void artdaq::database::conf::debug::ConfWriter() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TLOG(10) << "artdaq::database::conf::ConfWriter trace_enable";
}
