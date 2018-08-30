#ifndef _ARTDAQ_DATABASE_DATAFORMATS_FHICL_FHICL_READER_H_
#define _ARTDAQ_DATABASE_DATAFORMATS_FHICL_FHICL_READER_H_

#include "artdaq-database/DataFormats/Fhicl/fhicl_types.h"
#include "artdaq-database/DataFormats/Json/json_types.h"
#include "artdaq-database/DataFormats/common.h"

#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/support_line_pos_iterator.hpp>

namespace artdaq {
namespace database {
namespace fhicl {

using namespace boost::spirit;

using comment_t = std::string;
using linenum_t = integer;
using comments_t = std::map<linenum_t, comment_t>;
using linenum_comment_t = std::pair<linenum_t, comment_t>;

using include_t = std::string;
using includes_t = std::vector<include_t>;

using pos_iterator_t = boost::spirit::line_pos_iterator<std::string::const_iterator>;

struct get_line_f {
  template <typename>
  struct result {
    typedef size_t type;
  };
  template <typename Iter>
  size_t operator()(Iter const& pos_iter) const {
    return get_line(pos_iter);
  }
};

template <typename Iter>
struct fhicl_comments_parser_grammar : qi::grammar<Iter, comments_t(), qi::blank_type> {
  fhicl_comments_parser_grammar() : fhicl_comments_parser_grammar::base_type(comments_rule) {
    using boost::spirit::qi::_1;
    using boost::spirit::qi::_val;
    using boost::spirit::qi::eoi;
    using boost::spirit::qi::eol;
    using boost::spirit::qi::lit;
    using boost::spirit::qi::raw;

    using boost::phoenix::begin;
    using boost::phoenix::construct;
    using boost::phoenix::end;

    whitespace_rule = *(ascii::char_ - (lit("#") | lit("//")));

    whitespace_b4quotedstring_rule = *(ascii::char_ - (lit("\"") | lit("#") | lit("//")));

    quoted_string_rule = '"' >> +(ascii::char_ - '"') >> '"';

    padded_quoted_string_rule = whitespace_b4quotedstring_rule >> *(quoted_string_rule >> whitespace_b4quotedstring_rule);

    string_rule = +(ascii::char_ - (eol | eoi));

    comment_rule = raw[string_rule][_val = construct<linenum_comment_t>(get_line_(begin(_1)), construct<std::string>(begin(_1), end(_1)))];

    comments_rule = (padded_quoted_string_rule >> whitespace_rule >> comment_rule) % eol;
  }

  qi::rule<Iter> whitespace_rule, whitespace_b4quotedstring_rule, padded_quoted_string_rule, quoted_string_rule;
  qi::rule<Iter, std::string()> string_rule;
  qi::rule<Iter, linenum_comment_t()> comment_rule;
  qi::rule<Iter, comments_t(), qi::blank_type> comments_rule;

  boost::phoenix::function<get_line_f> get_line_;
};

template <typename Iter>
struct fhicl_includes_parser_grammar : qi::grammar<Iter, includes_t(), qi::blank_type> {
  fhicl_includes_parser_grammar() : fhicl_includes_parser_grammar::base_type(includes_rule) {
    using boost::spirit::qi::eoi;
    using boost::spirit::qi::eol;
    using boost::spirit::qi::lit;
    using boost::spirit::qi::omit;
    using boost::spirit::qi::space;

    include_rule = omit[*space] >> lit("#include") >> omit[*space];

    filename_rule = '"' >> +(ascii::char_ - '"') >> '"';

    includes_rule = (include_rule >> filename_rule) % eol;
  }

  qi::rule<Iter> include_rule;
  qi::rule<Iter, include_t()> filename_rule;
  qi::rule<Iter, includes_t(), qi::blank_type> includes_rule;
};

struct FhiclReader final {
  bool read_data(std::string const&, jsn::object_t&);
  bool read_comments(std::string const&, jsn::array_t&);
};

namespace debug {
void FhiclReader();
}
}  // namespace fhicl
}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_DATAFORMATS_FHICL_FHICL_READER_H_ */
