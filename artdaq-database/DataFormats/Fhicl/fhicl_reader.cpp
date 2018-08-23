#include "artdaq-database/DataFormats/common.h"

#include "artdaq-database/DataFormats/Fhicl/convertfhicl2jsondb.h"
#include "artdaq-database/DataFormats/Fhicl/fhicl_reader.h"
#include "artdaq-database/DataFormats/Fhicl/fhicl_types.h"
#include "artdaq-database/DataFormats/Fhicl/fhiclcpplib_includes.h"
#include "artdaq-database/DataFormats/Fhicl/helper_functions.h"
#include "artdaq-database/DataFormats/Json/json_types.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "fhicl_reader.cpp"

namespace jsn = artdaq::database::json;
namespace fcl = artdaq::database::fhicl;

namespace literal = artdaq::database::dataformats::literal;

using artdaq::database::fhicl::FhiclReader;

bool FhiclReader::read_data(std::string const& in, jsn::object_t& json_object) {
  confirm(!in.empty());
  confirm(json_object.empty());

  TLOG(20) << "read_data() begin";

  try {
    using boost::spirit::qi::blank;
    using boost::spirit::qi::phrase_parse;

    using artdaq::database::fhicl::fhicl_comments_parser_grammar;
    using artdaq::database::fhicl::pos_iterator_t;
    using artdaq::database::fhicljson::datapair_t;
    using artdaq::database::fhicljson::extra_opts;
    using artdaq::database::fhicljson::fcl2jsondb;

    auto comments = comments_t();
    fhicl_comments_parser_grammar<pos_iterator_t> grammar;
    auto start = pos_iterator_t(in.begin());
    auto end = pos_iterator_t(in.end());

    TLOG(21) << "read_data() parse comments begin";

    if (!phrase_parse(start, end, grammar, blank, comments)) {
      comments[0] = "No comments";
    }
    TLOG(22) << "read_data() parse comments end";

    auto object = jsn::object_t();

    auto make_SubNode = [](auto& parent, auto const& child_name) -> auto& {
      parent[child_name] = jsn::object_t();
      return boost::get<jsn::object_t>(parent[child_name]);
    };

    auto& data_node = make_SubNode(object, literal::data);
    auto& metadata_node = make_SubNode(object, literal::metadata);

    auto conf = std::string{in};
    conf.reserve(conf.size() + 1024);

    auto idx = std::size_t{0};

    auto regex = std::regex{"(#include\\s)([^'\"]*)"};

    TLOG(23) << "read_data() regex includes begin";
    
    std::for_each(std::sregex_iterator(conf.begin(), conf.end(), regex), std::sregex_iterator(),
                  [&conf, &idx](auto& m) {
                    conf.replace(m.position(), m.length(), "fhicl_pound_include_" + std::to_string(idx++) + ":");
                  });
    TLOG(24) << "read_data() regex includes end";

    ::fhicl::intermediate_table fhicl_table;

    ::shims::isSnippetMode(true);

    TLOG(25) << "read_data() parse_document begin";
    
    ::fhicl::parse_document(conf, fhicl_table);

    TLOG(26) << "read_data() parse_document end";

    TLOG(27) << "parse_document() returned " << std::distance(fhicl_table.begin(), fhicl_table.end()) << " entries.";

    {  // convert prolog section
      auto opts = extra_opts{};
      opts.enablePrologMode();

      auto& data = make_SubNode(data_node, literal::prolog);

      auto& prolog = make_SubNode(metadata_node, literal::prolog);
      prolog[literal::type] = std::string{literal::table};
      prolog[literal::comment] = std::string{"#"}.append(literal::prolog);
      //      prolog[literal::annotation] = std::string{"//Auto generated
      //      "}.append(literal::prolog);

      auto& metadata = make_SubNode(prolog, literal::children);
      
      TLOG(28) << "read_data() convert prolog begin";

      for (auto const& fhicl_element : fhicl_table) {
        if (fhicl_element.second.in_prolog && opts.readProlog) {
          datapair_t pair = fcl2jsondb(std::forward_as_tuple(fhicl_element, fhicl_element, comments, opts));
          data.push_back(pair.first);
          metadata.push_back(pair.second);
        }
      }
      TLOG(29) << "read_data() convert prolog end";
      
    }

    {  // convert main section
      auto opts = extra_opts{};
      opts.enableDefaultMode();

      auto& data = make_SubNode(data_node, literal::main);

      auto& main = make_SubNode(metadata_node, literal::main);
      main[literal::type] = std::string{literal::table};
      main[literal::comment] = std::string{"#"}.append(literal::main);
      //      main[literal::annotation] = std::string{"//Auto generated
      //      "}.append(literal::main);

      auto& metadata = make_SubNode(main, literal::children);
      
      TLOG(30) << "read_data() convert main begin";

      for (auto const& fhicl_element : fhicl_table) {
        if (!fhicl_element.second.in_prolog && opts.readMain) {
          datapair_t pair = fcl2jsondb(std::forward_as_tuple(fhicl_element, fhicl_element, comments, opts));
          data.push_back(pair.first);
          metadata.push_back(pair.second);
        }
      }
      TLOG(31) << "read_data() convert main end";
      
    }

    json_object.swap(object);

    TLOG(32) << "read_data() end";

    return true;

  } catch (::fhicl::exception const& e) {
    TLOG(33) << "read_data() Caught fhicl::exception message=" << e.what();

    std::cerr << "Caught fhicl::exception message=" << e.what() << "\n";
    throw;
  }
}

bool FhiclReader::read_comments(std::string const& in, jsn::array_t& json_array) {
  confirm(!in.empty());
  confirm(json_array.empty());

  TLOG(41) << "read_comments() begin";

  try {
    using boost::spirit::qi::blank;
    using boost::spirit::qi::phrase_parse;

    using artdaq::database::fhicl::fhicl_comments_parser_grammar;
    using artdaq::database::fhicl::pos_iterator_t;

    fhicl_comments_parser_grammar<pos_iterator_t> grammar;

    auto start = pos_iterator_t(in.begin());
    auto end = pos_iterator_t(in.end());

    auto comments = comments_t();
    
    TLOG(42) << "read_comments() phrase_parse begin";
    auto result = phrase_parse(start, end, grammar, blank, comments);
    TLOG(43) << "read_comments() phrase_parse end";

    if (!result) {
      comments[0] = "No comments";
      result = true;
    }

    auto array = jsn::array_t();
    
    TLOG(44) << "read_comments() integrate comments begin";

    for (auto const& comment : comments) {
      array.push_back(jsn::object_t());
      auto& object = boost::get<jsn::object_t>(array.back());
      object.push_back(jsn::data_t::make(literal::linenum, comment.first));

      if (fcl::from_json_string(fcl::to_json_string(comment.second)) != comment.second) {
        TLOG(45) << "Warning non reversable convertion\nbegin" << comment.second;
        TLOG(46) << "convd" << fcl::to_json_string(comment.second);
        TLOG(47) << "restd" << fcl::from_json_string(fcl::to_json_string(comment.second));
      }
      object.push_back(jsn::data_t::make(literal::value, fcl::to_json_string(comment.second)));
    }
    TLOG(48) << "read_comments() integrate comments end";

    json_array.swap(array);

    TLOG(49) << "read_comments() begin";

    return result;

  } catch (::fhicl::exception const& e) {
    std::cerr << "Caught fhicl::exception message=" << e.what() << "\n";
    throw;
  }
}

void artdaq::database::fhicl::debug::FhiclReader() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TLOG(50) << "artdaq::database::fhicl::FhiclReader trace_enable";
}
