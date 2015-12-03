#ifndef _ARTDAQ_DATABASE_FHICLJSON_JSON_READER_H_
#define _ARTDAQ_DATABASE_FHICLJSON_JSON_READER_H_

#include "artdaq-database/FhiclJson/common.h"
#include "artdaq-database/FhiclJson/json_types.h"
#include "artdaq-database/FhiclJson/shared_literals.h"

#include <boost/spirit/include/qi.hpp>

namespace artdaq{
namespace database{
namespace json{

using namespace boost::spirit;

template <typename Iter>
struct json_parser_grammar
: qi::grammar< Iter, table_t() , ascii::space_type > {
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

    key_rule      = quoted_string;

    atom_rule     = key_rule
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

    start         = table_rule;

    BOOST_SPIRIT_DEBUG_NODE(key_rule);
    BOOST_SPIRIT_DEBUG_NODE(value_rule);
    BOOST_SPIRIT_DEBUG_NODE(atom_rule);
    BOOST_SPIRIT_DEBUG_NODE(table_rule);
    BOOST_SPIRIT_DEBUG_NODE(sequence_rule);
    BOOST_SPIRIT_DEBUG_NODE(start);
}

qi::rule< Iter, std::string(),  ascii::space_type > quoted_string;
qi::rule< Iter, value_t(),      ascii::space_type > value_rule;
qi::rule< Iter, key_t(),        ascii::space_type > key_rule;
qi::rule< Iter, atom_t(),       ascii::space_type > atom_rule;
qi::rule< Iter, table_t(),      ascii::space_type > start, table_rule;
qi::rule< Iter, sequence_t(),   ascii::space_type > sequence_rule;
};

struct JsonReader final {
    bool read(std::string const&, table_t&);
};


} //namespace json
} //namespace database
} //namespace artdaq
#endif /* _ARTDAQ_DATABASE_FHICLJSON_JSON_READER_H_ */
