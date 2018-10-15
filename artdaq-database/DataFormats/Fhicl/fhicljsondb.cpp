#include "artdaq-database/DataFormats/common.h"
#include "artdaq-database/DataFormats/shared_literals.h"

#include "artdaq-database/DataFormats/Fhicl/fhiclcpplib_includes.h"
#include "artdaq-database/DataFormats/Fhicl/helper_functions.h"

#include "artdaq-database/DataFormats/Fhicl/fhicl_common.h"
#include "artdaq-database/DataFormats/Fhicl/fhicljsondb.h"
#include "artdaq-database/DataFormats/Json/json_common.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "fhicljsondb.cpp"

namespace dbfj = artdaq::database::fhicljson;

namespace literal = artdaq::database::dataformats::literal;

namespace jsn = artdaq::database::json;

using artdaq::database::json::JsonReader;
using artdaq::database::json::JsonWriter;

using artdaq::database::fhicl::FhiclReader;
using artdaq::database::fhicl::FhiclWriter;

namespace artdaq {
namespace database {
namespace fhicljson {
bool fhicl_to_ast(std::string const& fcl, std::string const& filename, jsn::object_t& json) __attribute__ ((visibility ("default"))) ;
}  // namespace fhicljson
}  // namespace database
}  // namespace artdaq

bool dbfj::fhicl_to_ast(std::string const& fcl, std::string const& filename, jsn::object_t& json) {
  confirm(!fcl.empty());
  confirm(!filename.empty());

  TLOG(20) << "fhicl_to_json: begin (ast)";

  shims::isSnippetMode(true);

  auto result = bool(false);

  auto json_root = jsn::object_t();

  auto get_object = [&json_root](std::string const& name) -> auto& { return boost::get<jsn::object_t>(json_root[name]); };

  json_root[literal::document] = jsn::object_t();
  json_root[literal::comments] = jsn::array_t();
  json_root[literal::origin] = jsn::object_t();

  json_root[literal::version] = std::string{literal::notprovided};
  json_root[literal::entities] = jsn::array_t();
  json_root[literal::configurations] = jsn::array_t();

  TLOG(21) << "fhicl_to_json: Created root nodes";

  get_object(literal::origin)[literal::format] = std::string("fhicl");
  get_object(literal::origin)[literal::name] = filename;
  get_object(literal::origin)[literal::source] = std::string("fhicl_to_json");
  get_object(literal::origin)[literal::timestamp] = artdaq::database::timestamp();

  auto reader = FhiclReader();

  TLOG(22) << "read_comments begin";
  result = reader.read_comments(fcl, boost::get<jsn::array_t>(json_root[literal::comments]));
  TLOG(23) << "read_comments end result=" << static_cast<int>(result);

  if (!result) {
    return result;
  }

  TLOG(24) << "read_data begin";
  result = reader.read_data(fcl, boost::get<jsn::object_t>(json_root[literal::document]));
  TLOG(25) << "read_data end result=" << static_cast<int>(result);

  if (!result) {
    return result;
  }

  if (result) {
    json.swap(json_root);
  }

  TLOG(28) << "fhicl_to_json: end (ast)";

  return result;
}

bool dbfj::fhicl_to_json(std::string const& fcl, std::string const& filename, std::string& json) {
  confirm(!fcl.empty());
  confirm(json.empty());
  confirm(!filename.empty());
  TLOG(41) << "fhicl_to_json: begin";

  auto json_root = jsn::object_t();

  bool result = dbfj::fhicl_to_ast(fcl, filename, json_root);

  if (!result) {
    return result;
  }

  auto json1 = std::string();

  auto writer = JsonWriter();

  TLOG(42) << "fhicl_to_json: write() begin";
  result = writer.write(json_root, json1);
  TLOG(43) << "fhicl_to_json: write() end";

  if (result) {
    json.swap(json1);
  }

  TLOG(44) << "fhicl_to_json: end";

  return result;
}

bool dbfj::json_to_fhicl(std::string const& json, std::string& fcl, std::string& filename) {
  confirm(!json.empty());
  confirm(fcl.empty());

  TLOG(30) << "json_to_fhicl: begin";

  auto result = bool(false);

  auto json_root = jsn::object_t();

  auto reader = JsonReader();

  TLOG(31) << "json_to_fhicl: Reading json root nodes";

  result = reader.read(json, json_root);

  if (!result) {
    TLOG(32) << "json_to_fhicl: Failed to read json root nodes";
    return result;
  }

  auto writer = FhiclWriter();

  auto fcl_data = std::string();
  auto const& document_object = boost::get<jsn::object_t>(json_root[literal::document]);

  filename = "notprovided";
  result = writer.write_data(document_object, fcl_data);

  if (!result) {
    return result;
  }

  fcl.swap(fcl_data);

  TLOG(33) << "json_to_fhicl: fcl=<" << fcl << ">";

  TLOG(13) << "json_to_fhicl: end";

  return result;
}

void dbfj::debug::FhiclJson() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TLOG(34) << "artdaq::database::fhicljson trace_enable";
}
