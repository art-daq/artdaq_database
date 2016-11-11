#include "artdaq-database/DataFormats/common/helper_functions.h"
#include "artdaq-database/DataFormats/common/shared_literals.h"

#include "artdaq-database/DataFormats/Bin/bin_common.h"
#include "artdaq-database/DataFormats/Json/json_common.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "CNF:binjsondb_C"

namespace artdaq {
namespace database {
namespace binjson {

namespace bin = artdaq::database::bin;
namespace jsn = artdaq::database::json;

using artdaq::database::json::JsonReader;
using artdaq::database::json::JsonWriter;

using artdaq::database::bin::BinReader;
using artdaq::database::bin::BinWriter;

namespace literal = artdaq::database::dataformats::literal;

bool bin_to_json(std::string const& bin, std::string& json) {
  assert(!bin.empty());
  assert(json.empty());

  TRACE_(2, "bin_to_json: begin");

  auto result = bool{false};

  auto json_root = jsn::object_t{};
  json_root[literal::document_node] = jsn::object_t{};
  json_root[literal::origin_node] = jsn::object_t{};

  auto get_object = [&json_root](std::string const& name) -> auto& {
    return boost::get<jsn::object_t>(json_root[name]);
  };
  get_object(literal::origin_node)[literal::format] = std::string("bin");
  get_object(literal::origin_node)[literal::source] = std::string("bin_to_json");
  get_object(literal::origin_node)[literal::timestamp] = artdaq::database::dataformats::timestamp();

  auto& json_node [[gnu::unused]] = get_object(literal::document_node);

  auto reader [[gnu::unused]] = bin::BinReader{};
  result = false;//reader.read(bin, json_node);

  if (!result) return result;

  auto json1 = std::string{};

  auto writer = JsonWriter{};

  result = writer.write(json_root, json1);

  if (result) json.swap(json1);

  TRACE_(2, "bin_to_json: end");

  return result;
}

bool json_to_bin(std::string const& json, std::string& bin) {
  assert(!json.empty());
  assert(bin.empty());

  TRACE_(3, "json_to_bin: begin");

  auto result = bool{false};

  TRACE_(3, "json_to_bin: Reading JSON buffer..");

  auto json_root = jsn::object_t{};
  auto reader = JsonReader{};
  result = reader.read(json, json_root);

  if (!result) {
    TRACE_(3, "json_to_bin: Unable to read JSON buffer");
    return result;
  }

  auto get_object = [&json_root](std::string const& name) -> auto& {
    return boost::get<jsn::object_t>(json_root.at(name));
  };

  auto& json_node = get_object(literal::document_node);

  auto bin1 = std::string{};

  auto writer = BinWriter{};

  result = writer.write(json_node, bin1);

  if (result) bin.swap(bin1);

  TRACE_(3, "json_to_bin: end");

  return result;
}

namespace debug {
void enableBinJson() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TRACE_(0, "artdaq::database::binjson trace_enable");
}
}
}  // namespace binjson
}  // namespace database
}  // namespace artdaq
