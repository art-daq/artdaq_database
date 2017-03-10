#include "artdaq-database/DataFormats/common.h"

#include "artdaq-database/DataFormats/Fhicl/fhiclcpplib_includes.h"

#include "artdaq-database/DataFormats/Fhicl/convertfhicl2jsondb.h"
#include "artdaq-database/DataFormats/Fhicl/fhicl_types.h"
#include "artdaq-database/DataFormats/Fhicl/fhicl_writer.h"
#include "artdaq-database/DataFormats/Json/json_types.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "FCL:FclWriter_C"

namespace fcl = artdaq::database::fhicl;
namespace jsn = artdaq::database::json;
namespace literal = artdaq::database::dataformats::literal;

using artdaq::database::fhicl::FhiclWriter;
using artdaq::database::fhicljson::json2fcldb;
using artdaq::database::fhicljson::extra_opts;

bool FhiclWriter::write_data(jsn::object_t const& json_object, std::string& out) {
  confirm(out.empty());
  confirm(!json_object.empty());

  TRACE_(2, "write_data() begin");

  using artdaq::database::fhicl::fhicl_generator_grammar;
  using artdaq::database::fhicljson::valuetuple_t;

  auto get_SubNode = [](auto& parent, auto const& child_name) -> auto const& {
    return boost::get<jsn::object_t>(parent.at(child_name));
  };

  auto const& data_node = get_SubNode(json_object, literal::data);
  auto const& metadata_node = get_SubNode(json_object, literal::metadata);

  auto buffer = std::string();

  auto sink = std::back_insert_iterator<std::string>(buffer);

  {  // convert prolog section
    auto fhicl_table = fcl::table_t();

    auto opts = extra_opts{};
    opts.enablePrologMode();

    auto& data = get_SubNode(data_node, literal::prolog);

    auto& prolog = get_SubNode(metadata_node, literal::prolog);
    auto& metadata = get_SubNode(prolog, literal::children);

    for (auto const& element : data) {
      valuetuple_t value_tuple = std::forward_as_tuple(element.key, element.value, metadata.at(element.key));
      fhicl_table.push_back(json2fcldb(std::make_tuple(value_tuple, value_tuple, opts)));
    }

    if (data.size()) {
      buffer.append("BEGIN_PROLOG\n");
      sink = std::back_insert_iterator<std::string>(buffer);

      fhicl_generator_grammar<decltype(sink)> grammar;

      if (!karma::generate(sink, grammar, fhicl_table)) return false;
      buffer.append("\nEND_PROLOG\n");
    }
  }

  {  // convert main section
    auto fhicl_table = fcl::table_t();

    auto opts = extra_opts{};
    opts.enableDefaultMode();

    auto& data = get_SubNode(data_node, literal::main);

    auto& main = get_SubNode(metadata_node, literal::main);
    auto& metadata = get_SubNode(main, literal::children);

    for (auto const& element : data) {
      valuetuple_t value_tuple = std::forward_as_tuple(element.key, element.value, metadata.at(element.key));
      fhicl_table.push_back(json2fcldb(std::make_tuple(value_tuple, value_tuple, opts)));
    }

    fhicl_generator_grammar<decltype(sink)> grammar;

    if (!karma::generate(sink, grammar, fhicl_table)) return false;
  }

  buffer.reserve(buffer.size() + 512);

  auto regex = std::regex{"(#include\\s*:)([^\"]*)"};

  std::for_each(std::sregex_iterator(buffer.begin(), buffer.end(), regex), std::sregex_iterator(),
                [&buffer](auto& m) { buffer.replace(m.position(), m.length(), "#include "); });

  out.swap(buffer);

  TRACE_(2, "write_data() end");

  return true;
}

void artdaq::database::fhicl::debug::FhiclWriter() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TRACE_(0, "artdaq::database::fhicl::FhiclWrite trace_enable");
}