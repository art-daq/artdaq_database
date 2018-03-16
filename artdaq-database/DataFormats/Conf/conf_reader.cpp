#include "artdaq-database/DataFormats/common.h"

#include "artdaq-database/DataFormats/Conf/conf_reader.h"
#include "artdaq-database/DataFormats/Conf/convertconf2json.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "CNF:ConfReader_C"

namespace fcl = artdaq::database::conf;
namespace jsn = artdaq::database::json;

using artdaq::database::conf::ConfReader;

namespace literal = artdaq::database::dataformats::literal;

bool ConfReader::read(std::string const& in[[gnu::unused]], jsn::object_t& json_object) {
  confirm(!in.empty());
  confirm(json_object.empty());

  TLOG(12) << "read() begin";

  try {
    auto object = jsn::object_t();
    object[literal::data] = jsn::object_t();

    // TODO: convert CONF into Json AST

    json_object.swap(object);

    TLOG(12) << "read() end";

    return true;

  } catch (std::exception const& e) {
    TLOG(12) << "read() Caughtexception message=" << e.what();

    std::cerr << "Caught exception message=" << e.what() << "\n";
    throw;
  }
}

void artdaq::database::conf::debug::ConfReader() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TLOG(10) <<  "artdaq::database::conf::ConfReader trace_enable";
}
