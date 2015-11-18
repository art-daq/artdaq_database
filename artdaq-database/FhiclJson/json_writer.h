#ifndef _ARTDAQ_DATABASE_FHICLJSON_JSON_WRITER_H_
#define _ARTDAQ_DATABASE_FHICLJSON_JSON_WRITER_H_

#include "artdaq-database/FhiclJson/common.h"
#include "artdaq-database/FhiclJson/fhicl_json.h"

#include <boost/spirit/include/karma.hpp>

namespace artdaq{
namespace database{
namespace fhicljson{

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
                      | karma::bool_
                      ;

    quoted_string =  '"'
                     << +ascii::char_
                     << '"';

    atom_rule =      quoted_string
                     << " : "
                     << value_rule;

    table_rule =     "{\n" <<
                     atom_rule % ",\n"
                     << "\n}";

    sequence_rule = "[\n"
                    << value_rule % ",\n"
                    << "\n]";

    start =      table_rule;
}

karma::rule< Iter, table_t() >     start;
karma::rule< Iter, std::string() > quoted_string;
karma::rule< Iter, any_value_t() > value_rule;
karma::rule< Iter, atom_t()      > atom_rule;
karma::rule< Iter, table_t()     > table_rule;
karma::rule< Iter, sequence_t()  > sequence_rule;
};


struct JsonWriter final {
    bool write(table_t const&, std::string&);
};

} //namespace fhicljson
} //namespace database
} //namespace artdaq
#endif /* _ARTDAQ_DATABASE_FHICLJSON_JSON_WRITER_H_ */
