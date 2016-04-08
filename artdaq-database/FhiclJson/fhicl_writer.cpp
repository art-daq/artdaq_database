#include "artdaq-database/FhiclJson/common.h"

#include "artdaq-database/FhiclJson/fhicl_writer.h"

#include "artdaq-database/FhiclJson/convertfhicljsondb.h"
#include "artdaq-database/FhiclJson/convertfhicljsongui.h"
#include "artdaq-database/FhiclJson/fhicl_types.h"
#include "artdaq-database/FhiclJson/healper_functions.h"
#include "artdaq-database/FhiclJson/json_types.h"
#include "artdaq-database/FhiclJson/shared_literals.h"

#include "fhiclcpp/exception.h"
#include "fhiclcpp/extended_value.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "FCL:FclWriter_C"

namespace fcl = artdaq::database::fhicl;
namespace jsn = artdaq::database::json;

using artdaq::database::fhicl::FhiclWriter;
using artdaq::database::fhicljson::json2fclgui;
using artdaq::database::fhicljson::json2fcldb;

bool FhiclWriter::write_data_db(jsn::object_t const& json_object, std::string& out) {
  assert(out.empty());
  assert(!json_object.empty());

  TRACE_(2, "write_data_db() begin");

  using artdaq::database::fhicl::fhicl_generator_grammar;
  using artdaq::database::fhicljson::valuetuple_t;

  auto const& data_node = boost::get<jsn::object_t>(json_object.at(literal::data_node));
  auto const& metadata_node = boost::get<jsn::object_t>(json_object.at(literal::metadata_node));

  auto result = bool(false);
  auto buffer = std::string();

  auto fhicl_table = fcl::table_t();

  TRACE_(2, "write_data_db() create fcl table begin");

  for (auto const& data : data_node) {
    valuetuple_t value_tuple = std::forward_as_tuple(data.key, data.value, metadata_node.at(data.key));
    fhicl_table.push_back(json2fcldb(value_tuple, value_tuple));
  }

  TRACE_(2, "write_data_db() create fcl table end");

  auto sink = std::back_insert_iterator<std::string>(buffer);

  fhicl_generator_grammar<decltype(sink)> grammar;

  result = karma::generate(sink, grammar, fhicl_table);

  if (result) out.swap(buffer);

  TRACE_(2, "write_data_db() end");

  return result;
}

bool FhiclWriter::write_data_gui(jsn::array_t const& json_array, std::string& out) {
  assert(out.empty());
  assert(!json_array.empty());

  using artdaq::database::fhicl::fhicl_generator_grammar;

  auto result = bool(false);
  auto buffer = std::string();

  auto fhicl_table = fcl::table_t();

  for (auto const& json_value : json_array) fhicl_table.push_back(json2fclgui(json_value, json_value));

  auto sink = std::back_insert_iterator<std::string>(buffer);

  fhicl_generator_grammar<decltype(sink)> grammar;

  result = karma::generate(sink, grammar, fhicl_table);

  if (result) out.swap(buffer);

  return result;
}

bool FhiclWriter::write_includes(jsn::array_t const& json_array, std::string& out) {
  assert(out.empty());
  assert(!json_array.empty());

  using artdaq::database::fhicl::fhicl_include_generator_grammar;

  auto result = bool(false);
  auto buffer = std::string();

  auto includes = includes_t();

  includes.reserve(json_array.size());

  for (auto const& json_value : json_array)
    includes.push_back(artdaq::database::fhicljson::unescape(boost::get<std::string>(json_value)));

  auto sink = std::back_insert_iterator<std::string>(buffer);

  fhicl_include_generator_grammar<decltype(sink)> grammar;

  result = karma::generate(sink, grammar, includes);

  if (result) out.swap(buffer);

  return result;
}

void artdaq::database::fhicl::trace_enable_FhiclWriter() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", 1LL);
  TRACE_CNTL("modeS", 1LL);

  TRACE_(0, "artdaq::database::fhicl::FhiclWriter"
                << "trace_enable");
}
