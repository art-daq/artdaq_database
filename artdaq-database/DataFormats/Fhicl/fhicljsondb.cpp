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

#define TRACE_NAME "FHJS:fhcljsndb_C"

namespace dbfj = artdaq::database::fhicljson;

namespace literal = artdaq::database::dataformats::literal;

namespace fcl = artdaq::database::fhicl;
namespace jsn = artdaq::database::json;

using artdaq::database::json::JsonReader;
using artdaq::database::json::JsonWriter;

using artdaq::database::fhicl::FhiclReader;
using artdaq::database::fhicl::FhiclWriter;

bool dbfj::fhicl_to_json(std::string const& fcl, std::string const& filename, std::string& json) {
  confirm(!fcl.empty());
  confirm(json.empty());
  confirm(!filename.empty());

  TLOG(12) << "fhicl_to_json: begin";

  shims::isSnippetMode(true);

  auto result = bool(false);

  auto json_root = jsn::object_t();

  auto get_object = [&json_root](std::string const& name) -> auto& {
    return boost::get<jsn::object_t>(json_root[name]);
  };

  json_root[literal::document] = jsn::object_t();
  json_root[literal::comments] = jsn::array_t();
  json_root[literal::origin] = jsn::object_t();

  json_root[literal::version] = std::string{literal::notprovided};
  json_root[literal::entities] = jsn::array_t();
  json_root[literal::configurations] = jsn::array_t();

  TLOG(12) << "fhicl_to_json: Created root nodes";

  get_object(literal::origin)[literal::format] = std::string("fhicl");
  get_object(literal::origin)[literal::name] = filename;
  get_object(literal::origin)[literal::source] = std::string("fhicl_to_json");
  get_object(literal::origin)[literal::timestamp] = artdaq::database::timestamp();

  auto reader = FhiclReader();

  TLOG(12) << "read_comments begin";
  result = reader.read_comments(fcl, boost::get<jsn::array_t>(json_root[literal::comments]));
  TLOG(12) << "read_comments end result=" << std::to_string(result);

  if (!result) return result;

  TLOG(12) << "read_data begin";
  result = reader.read_data(fcl, boost::get<jsn::object_t>(json_root[literal::document]));
  TLOG(12) << "read_data end result=" << std::to_string(result);

  if (!result) return result;

  auto json1 = std::string();

  auto writer = JsonWriter();

  TLOG(12) << "fhicl_to_json: write() begin";
  result = writer.write(json_root, json1);
  TLOG(12) << "fhicl_to_json: write() end";

  if (result) json.swap(json1);

  TLOG(12) << "fhicl_to_json: end";

  return result;
}

bool dbfj::json_to_fhicl(std::string const& json, std::string& fcl, std::string& filename) {
  confirm(!json.empty());
  confirm(fcl.empty());

  TLOG(13)<< "json_to_fhicl: begin";

  auto result = bool(false);

  auto json_root = jsn::object_t();

  auto reader = JsonReader();

  TLOG(13)<< "json_to_fhicl: Reading json root nodes";

  result = reader.read(json, json_root);

  if (!result) {
    TLOG(13)<< "json_to_fhicl: Failed to read json root nodes";
    return result;
  }

  auto writer = FhiclWriter();

  auto fcl_data = std::string();
  auto const& document_object = boost::get<jsn::object_t>(json_root[literal::document]);

  filename = "notprovided";
  result = writer.write_data(document_object, fcl_data);

  if (!result) return result;

  fcl.swap(fcl_data);

  TLOG(13)<< "json_to_fhicl: fcl=<" << fcl << ">";

  TLOG(13)<< "json_to_fhicl: end";

  return result;
}

void dbfj::debug::FhiclJson() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TLOG(10) <<  "artdaq::database::fhicljson trace_enable";
}
