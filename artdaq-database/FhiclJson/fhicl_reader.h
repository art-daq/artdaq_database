#ifndef _ARTDAQ_DATABASE_FHICLJSON_FHICL_READER_H_
#define _ARTDAQ_DATABASE_FHICLJSON_FHICL_READER_H_

#include "artdaq-database/FhiclJson/common.h"
#include "artdaq-database/FhiclJson/fhicl_types.h"
#include "artdaq-database/FhiclJson/json_types.h"

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/support_line_pos_iterator.hpp>

namespace artdaq{
namespace database{
namespace fhicl{

namespace adf = artdaq::database::fhicl;
namespace adj = artdaq::database::json;

using namespace boost::spirit;

using comment_t = std::string;
using linenum_t = int;
using comments_t = std::map<linenum_t, comment_t>;
using linenum_comment_t = std::pair<linenum_t, comment_t>;

using pos_iterator_t = boost::spirit::line_pos_iterator<std::string::const_iterator>;

struct get_line_f {
    template <typename> struct result {
        typedef size_t type;
    };
    template <typename Iter> size_t operator()(Iter const& pos_iter) const {
        return  get_line(pos_iter);
    }
};

template <typename Iter>
struct fhicl_comments_parser_grammar
: qi::grammar<Iter, comments_t() , qi::blank_type > {

    fhicl_comments_parser_grammar()
        : fhicl_comments_parser_grammar::base_type(comments_rule) {
    using boost::spirit::qi::_val;
    using boost::spirit::qi::_1;
    using boost::spirit::qi::lit;
    using boost::spirit::qi::eol;
    using boost::spirit::qi::eoi;
    using boost::spirit::qi::raw;

    using  boost::phoenix::construct;
    using  boost::phoenix::begin;
    using  boost::phoenix::end;

    whitespace_rule = *(ascii::char_ - (lit("#") | lit("//")));

    whitespace_b4quotedstring_rule =  *(ascii::char_ - (lit("\"") | lit("#") | lit("//")));

    quoted_string_rule =  '"' >> +(ascii::char_ - '"') >> '"';

    padded_quoted_string_rule = whitespace_b4quotedstring_rule >> *(quoted_string_rule >> whitespace_b4quotedstring_rule);

    string_rule = +(ascii::char_ - (eol | eoi));

    comment_rule  = raw[ string_rule ]
                    [ _val = construct<linenum_comment_t>(get_line_(begin(_1)),
                             construct<std::string>(begin(_1), end(_1))) ];

    comments_rule = (padded_quoted_string_rule >> whitespace_rule >>  comment_rule) %  eol ;
}

qi::rule< Iter>                                whitespace_rule, whitespace_b4quotedstring_rule, padded_quoted_string_rule, quoted_string_rule;
qi::rule< Iter, std::string()>                 string_rule;
qi::rule< Iter, linenum_comment_t()>           comment_rule;
qi::rule< Iter, comments_t(), qi::blank_type>  comments_rule;

boost::phoenix::function<get_line_f> get_line_;

};

namespace adj = artdaq::database::json;

struct FhiclReader final {
    bool read(std::string const&, adj::sequence_t&);
    bool read_comments(std::string const&, comments_t&);
};

} //namespace fhicl
} //namespace database
} //namespace artdaq
#endif /* _ARTDAQ_DATABASE_FHICLJSON_FHICL_READER_H_ */
