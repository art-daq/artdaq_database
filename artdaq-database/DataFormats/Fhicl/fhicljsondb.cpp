#include "artdaq-database/DataFormats/common.h"
#include "artdaq-database/DataFormats/common/helper_functions.h"

#include "artdaq-database/DataFormats/Fhicl/fhiclcpplib_includes.h"
#include "artdaq-database/DataFormats/Fhicl/helper_functions.h"

#include "artdaq-database/DataFormats/Fhicl/fhicl_common.h"
#include "artdaq-database/DataFormats/Json/json_common.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "FHJS:fhicljsondb_C"

namespace artdaq {
namespace database {
namespace fhicljson {

namespace literal = artdaq::database::dataformats::literal;

namespace fcl = artdaq::database::fhicl;
namespace jsn = artdaq::database::json;

using artdaq::database::json::JsonReader;
using artdaq::database::json::JsonWriter;

using artdaq::database::fhicl::FhiclReader;
using artdaq::database::fhicl::FhiclWriter;

bool fhicl_to_json(std::string const& fcl, std::string& json) {
  assert(!fcl.empty());
  assert(json.empty());

  TRACE_(2, "fhicl_to_json: begin");

  shims::isSnippetMode(true);

  auto result = bool(false);

  auto json_root = jsn::object_t();

  auto get_object = [&json_root](std::string const& name) -> auto& {
    return boost::get<jsn::object_t>(json_root[name]);
  };

  json_root[literal::document_node] = jsn::object_t();
  json_root[literal::comments_node] = jsn::array_t();
  json_root[literal::origin_node] = jsn::object_t();
  
  json_root[literal::version_node] = std::string{literal::notprovided};
  json_root[literal::configurable_entity_node] =  jsn::object_t();
  json_root[literal::configurations_node] =  jsn::array_t();
  
  TRACE_(2, "fhicl_to_json: Created root nodes");

  get_object(literal::origin_node)[literal::format] = std::string("fhicl");
  get_object(literal::origin_node)[literal::source] = std::string("fhicl_to_json");
  get_object(literal::origin_node)[literal::timestamp] = artdaq::database::dataformats::timestamp();

  get_object(literal::configurable_entity_node)[literal::name]=std::string{literal::notprovided};
  
  auto reader = FhiclReader();

  TRACE_(2, "read_comments begin");
  result = reader.read_comments(fcl, boost::get<jsn::array_t>(json_root[literal::comments_node]));
  TRACE_(2, "read_comments end result=" << std::to_string(result));

  if (!result) return result;

  TRACE_(2, "read_data begin");
  result = reader.read_data(fcl, boost::get<jsn::object_t>(json_root[literal::document_node]));
  TRACE_(2, "read_data end result=" << std::to_string(result));

  if (!result) return result;

  auto json1 = std::string();

  auto writer = JsonWriter();

  TRACE_(2, "fhicl_to_json: write() begin");
  result = writer.write(json_root, json1);
  TRACE_(2, "fhicl_to_json: write() end");

  if (result) json.swap(json1);

  TRACE_(2, "fhicl_to_json: end");

  return result;
}

bool json_to_fhicl(std::string const& json, std::string& fcl) {
  assert(!json.empty());
  assert(fcl.empty());

  TRACE_(3, "json_to_fhicl: begin");

  auto result = bool(false);

  auto json_root = jsn::object_t();

  auto reader = JsonReader();

  TRACE_(3, "json_to_fhicl: Reading json root nodes");

  result = reader.read(json, json_root);

  if (!result) {
    TRACE_(3, "json_to_fhicl: Failed to read json root nodes");
    return result;
  }

  auto writer = FhiclWriter();

  auto fcl_data = std::string();
  auto const& document_object = boost::get<jsn::object_t>(json_root[literal::document_node]);

  result = writer.write_data(document_object, fcl_data);

  if (!result) return result;

  fcl.swap(fcl_data);

  TRACE_(3, "json_to_fhicl: fcl=<" << fcl << ">");

  TRACE_(3, "json_to_fhicl: end");

  return result;
}

namespace debug {
void enableFhiclJson() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TRACE_(0, "artdaq::database::fhicljson trace_enable");
}
}
}  // namespace fhicljson
}  // namespace database
}  // namespace artdaq
