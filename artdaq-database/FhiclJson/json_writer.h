#ifndef _ARTDAQ_DATABASE_FHICLJSON_JSON_WRITER_H_
#define _ARTDAQ_DATABASE_FHICLJSON_JSON_WRITER_H_

#include "artdaq-database/FhiclJson/common.h"
#include "artdaq-database/FhiclJson/json_types.h"

#include <boost/spirit/include/karma.hpp>

namespace artdaq{
namespace database{
namespace json{

using namespace boost::spirit;

template <typename Iter>
struct json_generator_grammar
: karma::grammar< Iter, table_t() > {
    json_generator_grammar()
        : json_generator_grammar::base_type(start) {
    value_rule    =   table_rule
                      | sequence_rule 
                      | quoted_string
                      | karma::int_
                      | karma::double_
                      | karma::bool_;

    quoted_string =  '"'
                     << karma::string
                     << '"';

    key_rule  =      quoted_string;

    atom_rule =      key_rule
                     << " : "
                     << value_rule;


    table_rule =     "{\n" <<
                     atom_rule % ",\n"
                     << "\n}";


    sequence_rule = "[\n"
                    << value_rule % ",\n"
                    << "\n]";

    start =      table_rule;

    BOOST_SPIRIT_DEBUG_NODE(key_rule);
    BOOST_SPIRIT_DEBUG_NODE(value_rule);
    BOOST_SPIRIT_DEBUG_NODE(atom_rule);
    BOOST_SPIRIT_DEBUG_NODE(table_rule);
    BOOST_SPIRIT_DEBUG_NODE(sequence_rule);
    BOOST_SPIRIT_DEBUG_NODE(start);
}

karma::rule< Iter, std::string()   > quoted_string;
karma::rule< Iter, value_t()       > value_rule;
karma::rule< Iter, key_t()         > key_rule;
karma::rule< Iter, atom_t()        > atom_rule;
karma::rule< Iter, table_t()       > start, table_rule;
karma::rule< Iter, sequence_t()    > sequence_rule;
};

struct JsonWriter final {
    bool write(table_t const&, std::string&);
};

} //namespace json
} //namespace database
} //namespace artdaq
#endif /* _ARTDAQ_DATABASE_FHICLJSON_JSON_WRITER_H_ */
