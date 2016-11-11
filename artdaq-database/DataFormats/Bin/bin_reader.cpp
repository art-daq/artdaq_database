#include "artdaq-database/DataFormats/common.h"

#include "artdaq-database/DataFormats/Bin/convertbin2json.h"
#include "artdaq-database/DataFormats/Bin/bin_reader.h"
#include "artdaq-database/DataFormats/common/shared_literals.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "BIN:BinReader_C"

namespace fcl = artdaq::database::bin;
namespace jsn = artdaq::database::json;

using artdaq::database::bin::BinReader;

namespace literal = artdaq::database::dataformats::literal;

bool BinReader::read(std::vector<unsigned char> const& in [[gnu::unused]], jsn::object_t& json_object) {
  assert(!in.empty());
  assert(json_object.empty());

  TRACE_(2, "read() begin");

  try {
    auto object = jsn::object_t();
    object[literal::data_node] = jsn::object_t();

    // TODO: convert BIN into Json AST

    json_object.swap(object);

    TRACE_(2, "read() end");

    return true;

  } catch (std::exception const& e) {
    TRACE_(2, "read() Caughtexception message=" << e.what());

    std::cerr << "Caught exception message=" << e.what() << "\n";
    throw;
  }
}

void artdaq::database::bin::debug::enableBinReader() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TRACE_(0, "artdaq::database::bin::BinReader trace_enable");
}
