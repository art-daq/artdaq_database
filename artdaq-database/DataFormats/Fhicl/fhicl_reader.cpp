#include "artdaq-database/DataFormats/common.h"
#include "artdaq-database/DataFormats/common/helper_functions.h"
#include "artdaq-database/DataFormats/common/shared_literals.h"

#include "artdaq-database/DataFormats/Fhicl/convertfhicl2jsondb.h"
#include "artdaq-database/DataFormats/Fhicl/fhicl_reader.h"
#include "artdaq-database/DataFormats/Fhicl/fhicl_types.h"
#include "artdaq-database/DataFormats/Fhicl/fhiclcpplib_includes.h"
#include "artdaq-database/DataFormats/Fhicl/helper_functions.h"
#include "artdaq-database/DataFormats/Json/json_types.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "FCL:FclReader_C"

namespace jsn = artdaq::database::json;

namespace literal = artdaq::database::dataformats::literal;

using artdaq::database::fhicl::FhiclReader;

bool FhiclReader::read_data(std::string const& in, jsn::object_t& json_object) {
  assert(!in.empty());
  assert(json_object.empty());

  TRACE_(2, "read_data() begin");

  try {
    using boost::spirit::qi::phrase_parse;
    using boost::spirit::qi::blank;

    using artdaq::database::fhicl::fhicl_comments_parser_grammar;
    using artdaq::database::fhicl::pos_iterator_t;
    using artdaq::database::fhicljson::fcl2jsondb;
    using artdaq::database::fhicljson::extra_opts;
    using artdaq::database::fhicljson::datapair_t;

    auto comments = comments_t();
    fhicl_comments_parser_grammar<pos_iterator_t> grammar;
    auto start = pos_iterator_t(in.begin());
    auto end = pos_iterator_t(in.end());

    if (!phrase_parse(start, end, grammar, blank, comments)) comments[0] = "No comments";

    auto object = jsn::object_t();

    auto make_SubNode = [](auto& parent, auto const& child_name) -> auto& {
      parent[child_name] = jsn::object_t();
      return boost::get<jsn::object_t>(parent[child_name]);
    };

    auto& data_node = make_SubNode(object, literal::data_node);
    auto& metadata_node = make_SubNode(object, literal::metadata_node);

    auto conf = std::string{in};
    conf.reserve(conf.size() + 1024);

    auto idx = std::size_t{0};

    std::for_each(std::sregex_iterator(conf.begin(), conf.end(), std::regex{"(#include\\s)([^'\"]*)"}),
                  std::sregex_iterator(), [&conf, &idx](auto& m) {
                    conf.replace(m.position(), m.length(), "fhicl_pound_include_" + std::to_string(idx++) + ":");
                  });

    ::fhicl::intermediate_table fhicl_table;

    ::shims::isSnippetMode(true);

    ::fhicl::parse_document(conf, fhicl_table);

    TRACE_(2, "parse_document() returned " << std::distance(fhicl_table.begin(), fhicl_table.end()) << " entries.");

    {  // convert prolog section
      auto opts = extra_opts{};
      opts.enablePrologMode();

      auto& data = make_SubNode(data_node, literal::prolog_node);

      auto& prolog = make_SubNode(metadata_node, literal::prolog_node);
      prolog[literal::type] = std::string{literal::table};
      prolog[literal::comment] = std::string{"#"}.append(literal::prolog_node);
//      prolog[literal::annotation] = std::string{"//Auto generated "}.append(literal::prolog_node);

      auto& metadata = make_SubNode(prolog, literal::children);

      for (auto const& fhicl_element : fhicl_table) {
        if (fhicl_element.second.in_prolog && opts.readProlog) {
          datapair_t pair = std::move(fcl2jsondb(std::make_tuple(fhicl_element, fhicl_element, comments, opts)));
          data.push_back(std::move(pair.first));
          metadata.push_back(std::move(pair.second));
        }
      }
    }

    {  // convert main section
      auto opts = extra_opts{};
      opts.enableDefaultMode();

      auto& data = make_SubNode(data_node, literal::main_node);

      auto& main = make_SubNode(metadata_node, literal::main_node);
      main[literal::type] = std::string{literal::table};
      main[literal::comment] = std::string{"#"}.append(literal::main_node);
//      main[literal::annotation] = std::string{"//Auto generated "}.append(literal::main_node);

      auto& metadata = make_SubNode(main, literal::children);

      for (auto const& fhicl_element : fhicl_table) {
        if (!fhicl_element.second.in_prolog && opts.readMain) {
          datapair_t pair = std::move(fcl2jsondb(std::make_tuple(fhicl_element, fhicl_element, comments, opts)));
          data.push_back(std::move(pair.first));
          metadata.push_back(std::move(pair.second));
        }
      }
    }

    json_object.swap(object);

    TRACE_(2, "read_data() end");

    return true;

  } catch (::fhicl::exception const& e) {
    TRACE_(2, "read_data() Caught fhicl::exception message=" << e.what());

    std::cerr << "Caught fhicl::exception message=" << e.what() << "\n";
    throw;
  }
}

bool FhiclReader::read_comments(std::string const& in, jsn::array_t& json_array) {
  assert(!in.empty());
  assert(json_array.empty());

  try {
    using boost::spirit::qi::phrase_parse;
    using boost::spirit::qi::blank;

    using artdaq::database::fhicl::fhicl_comments_parser_grammar;
    using artdaq::database::fhicl::pos_iterator_t;

    fhicl_comments_parser_grammar<pos_iterator_t> grammar;

    auto start = pos_iterator_t(in.begin());
    auto end = pos_iterator_t(in.end());

    auto comments = comments_t();

    auto result = phrase_parse(start, end, grammar, blank, comments);

    if (!result) {
      comments[0] = "No comments";
      result = true;
    }

    auto array = jsn::array_t();

    for (auto const& comment : comments) {
      array.push_back(jsn::object_t());
      auto& object = boost::get<jsn::object_t>(array.back());
      object.push_back(jsn::data_t::make(literal::linenum, comment.first));
      object.push_back(
          jsn::data_t::make(literal::value, artdaq::database::dataformats::filter_jsonstring(comment.second)));
    }

    json_array.swap(array);

    return result;

  } catch (::fhicl::exception const& e) {
    std::cerr << "Caught fhicl::exception message=" << e.what() << "\n";
    throw;
  }
}

void artdaq::database::fhicl::debug::enableFhiclReader() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", 1LL);
  TRACE_CNTL("modeS", 1LL);

  TRACE_(0, "artdaq::database::fhicl::FhiclReader trace_enable");
}
