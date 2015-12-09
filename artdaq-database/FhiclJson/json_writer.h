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
: karma::grammar< Iter, object_t() > {
    json_generator_grammar()
        : json_generator_grammar::base_type(start) {
    value_rule    =   object_rule
                      | array_rule
                      | quoted_string
                      | karma::int_
                      | karma::double_
                      | karma::bool_;

    quoted_string =  '"'
                     << karma::string
                     << '"';

    key_rule  =      quoted_string;

    data_rule =      key_rule
                     << " : "
                     << value_rule ;


    object_rule =     "{\n" <<
                      data_rule % ",\n"
                      << "\n}";

    array_rule = "[\n"
                 << value_rule % ",\n"
                 << "\n]";

    start =      object_rule;

    BOOST_SPIRIT_DEBUG_NODE(key_rule);
    BOOST_SPIRIT_DEBUG_NODE(value_rule);
    BOOST_SPIRIT_DEBUG_NODE(data_rule);
    BOOST_SPIRIT_DEBUG_NODE(object_rule);
    BOOST_SPIRIT_DEBUG_NODE(array_rule);
    BOOST_SPIRIT_DEBUG_NODE(start);
}

karma::rule< Iter, std::string()   > quoted_string;
karma::rule< Iter, value_t()       > value_rule;
karma::rule< Iter, key_t()         > key_rule;
karma::rule< Iter, data_t()        > data_rule;
karma::rule< Iter, object_t()      > start, object_rule;
karma::rule< Iter, array_t()       > array_rule;
};

struct JsonWriter final {
    bool write(object_t const&, std::string&);
};

} //namespace json
} //namespace database
} //namespace artdaq
#endif /* _ARTDAQ_DATABASE_FHICLJSON_JSON_WRITER_H_ */
