#include "artdaq-database/DataFormats/common.h"

#include "artdaq-database/DataFormats/Conf/conf_common.h"
#include "artdaq-database/DataFormats/Json/json_common.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "CNF:confjsondb_C"

namespace artdaq {
namespace database {
namespace confjson {

namespace fcl = artdaq::database::conf;
namespace jsn = artdaq::database::json;

using artdaq::database::json::JsonReader;
using artdaq::database::json::JsonWriter;

using artdaq::database::conf::ConfReader;
using artdaq::database::conf::ConfWriter;

namespace literal = artdaq::database::dataformats::literal;

bool conf_to_json(std::string const& conf, std::string& json) {
  confirm(!conf.empty());
  confirm(json.empty());

  TLOG(2) << "conf_to_json: begin";

  auto result = bool{false};

  auto json_root = jsn::object_t{};
  json_root[literal::document] = jsn::object_t{};
  json_root[literal::origin] = jsn::object_t{};

  auto get_object = [&json_root](std::string const& name) -> auto& {
    return boost::get<jsn::object_t>(json_root[name]);
  };
  get_object(literal::origin)[literal::format] = std::string("conf");
  get_object(literal::origin)[literal::source] = std::string("conf_to_json");
  get_object(literal::origin)[literal::timestamp] = artdaq::database::timestamp();

  auto& json_node = get_object(literal::document);

  auto reader = conf::ConfReader{};
  result = reader.read(conf, json_node);

  if (!result) return result;

  auto json1 = std::string{};

  auto writer = JsonWriter{};

  result = writer.write(json_root, json1);

  if (result) json.swap(json1);

  TLOG(2) << "conf_to_json: end";

  return result;
}

bool json_to_conf(std::string const& json, std::string& conf) {
  confirm(!json.empty());
  confirm(conf.empty());

  TLOG(3)<< "json_to_conf: begin";

  auto result = bool{false};

  TLOG(3)<< "json_to_conf: Reading JSON buffer..";

  auto json_root = jsn::object_t{};
  auto reader = JsonReader{};
  result = reader.read(json, json_root);

  if (!result) {
    TLOG(3)<< "json_to_conf: Unable to read JSON buffer";
    return result;
  }

  auto get_object = [&json_root](std::string const& name) -> auto& {
    return boost::get<jsn::object_t>(json_root.at(name));
  };

  auto& json_node = get_object(literal::document);

  auto conf1 = std::string{};

  auto writer = ConfWriter{};

  result = writer.write(json_node, conf1);

  if (result) conf.swap(conf1);

  TLOG(3)<< "json_to_conf: end";

  return result;
}

namespace debug {
void enableConfJson() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TLOG(0) <<  "artdaq::database::confjson trace_enable";
}
}
}  // namespace confjson
}  // namespace database
}  // namespace artdaq
