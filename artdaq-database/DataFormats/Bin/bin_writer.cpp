#include "artdaq-database/DataFormats/common.h"

#include "artdaq-database/DataFormats/Bin/convertbin2json.h"
#include "artdaq-database/DataFormats/Bin/bin_writer.h"
#include "artdaq-database/DataFormats/common/shared_literals.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "BIN:BinWriter_C"

namespace fcl = artdaq::database::bin;
namespace jsn = artdaq::database::json;

using artdaq::database::bin::BinWriter;

namespace literal = artdaq::database::dataformats::literal;

bool BinWriter::write(jsn::object_t const& json_object, std::string& out) {
  assert(out.empty());
  assert(!json_object.empty());

  TRACE_(2, "write() begin");

  auto const& data_node = boost::get<jsn::object_t>(json_object.at(literal::data_node));

  auto result = bool(false);
  auto buffer = std::string();

  TRACE_(2, "write() create bin begin");

  for (auto const& data[[gnu::unused]] : data_node) {
    // TODO loop over json AST and convert it to XML
  }

  TRACE_(2, "write() create bin end");

  if (result) out.swap(buffer);

  TRACE_(2, "write() end");

  return result;
}

void artdaq::database::bin::debug::enableBinWriter() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TRACE_(0, "artdaq::database::bin::BinWriter trace_enable");
}
