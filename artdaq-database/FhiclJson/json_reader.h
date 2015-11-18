#ifndef _ARTDAQ_DATABASE_FHICLJSON_JSON_READER_H_
#define _ARTDAQ_DATABASE_FHICLJSON_JSON_READER_H_

#include "artdaq-database/FhiclJson/common.h"
#include "artdaq-database/FhiclJson/fhicl_json.h"

#include <boost/spirit/include/qi.hpp>

namespace artdaq{
namespace database{
namespace fhicljson{

using namespace boost::spirit;

template <typename Iter>
struct json_parser_grammar
: qi::grammar< Iter, any_value_t() , ascii::space_type > {
    json_parser_grammar()
        : json_parser_grammar::base_type(start) {
    quoted_string = qi::lexeme[
                        '"'
                        >> +(ascii::char_ - '"')
                        >> '"'
                    ];

    table_rule   = '{'
                   >> atom_rule % ','
                   >> '}';

    atom_rule     = quoted_string
                    >> ':'
                    >> value_rule;

    sequence_rule =  '['
                     >> value_rule % ','
                     >> ']';

    value_rule =     table_rule
                     | sequence_rule
                     | quoted_string
                     | (qi::int_ >> ! qi::lit('.'))
                     | qi::double_
                     | qi::bool_ ;

    start         = value_rule;
}

qi::rule< Iter, any_value_t(), ascii::space_type > start;
qi::rule< Iter, std::string(), ascii::space_type > quoted_string;
qi::rule< Iter, any_value_t(), ascii::space_type > value_rule;
qi::rule< Iter, atom_t(),      ascii::space_type > atom_rule;
qi::rule< Iter, table_t(),     ascii::space_type > table_rule;
qi::rule< Iter, sequence_t(),  ascii::space_type > sequence_rule;
};

struct JsonReader final {
    bool read(std::string const&, fhicljson::any_value_t&);
};


} //namespace fhicljson
} //namespace database
} //namespace artdaq
#endif /* _ARTDAQ_DATABASE_FHICLJSON_JSON_READER_H_ */
