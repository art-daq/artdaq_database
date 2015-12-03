#ifndef _ARTDAQ_DATABASE_FHICLJSON_FHICL_WRITER_H_
#define _ARTDAQ_DATABASE_FHICLJSON_FHICL_WRITER_H_

#include "artdaq-database/FhiclJson/common.h"
#include "artdaq-database/FhiclJson/fhicl_types.h"
#include "artdaq-database/FhiclJson/json_types.h"

#include <boost/spirit/include/karma.hpp>

namespace artdaq{
namespace database{
namespace fhicl{

namespace adf = artdaq::database::fhicl;
namespace adj = artdaq::database::json;

using namespace boost::spirit;

template <typename Iter>
struct fhicl_generator_grammar
: karma::grammar< Iter, table_t() > {
    fhicl_generator_grammar()
        : fhicl_generator_grammar::base_type(start) {
    variant_value_rule    =   table_rule
                              | sequence_rule
                              | quoted_string
                              | karma::int_
                              | karma::double_
                              | karma::bool_
                              ;

    annotated_value_rule = variant_value_rule << -annotation;

    string        =  karma::string;

    quoted_string =  '"'
                     << string
                     << '"';

    comment       =  "\n"
                     << string
                     << "\n";

    annotation    =  string
                     << "\n";

    commented_key_rule  = -comment << string;

    annotated_atom_rule     =  commented_key_rule
                               << " : "
                               << annotated_value_rule;

    table_rule =     "{\n" <<
                     annotated_atom_rule % ",\n"
                     << "\n}";

    toplevel_table = annotated_atom_rule % ",\n";

    sequence_rule = "[\n"
                    << annotated_value_rule % ",\n"
                    << "\n]";

    start =      toplevel_table;
}

karma::rule< Iter, std::string()       >    string, quoted_string, comment, annotation;
karma::rule< Iter, variant_value_t()   >    variant_value_rule;
karma::rule< Iter, value_t()           >    annotated_value_rule;
karma::rule< Iter, key_t()             >    commented_key_rule;
karma::rule< Iter, atom_t()            >    annotated_atom_rule;
karma::rule< Iter, table_t()           >    start, table_rule, toplevel_table;
karma::rule< Iter, sequence_t()        >    sequence_rule;
};

struct FhiclWriter final {
    bool write(adj::table_t const&, std::string&);
};

} //namespace fhicl
} //namespace database
} //namespace artdaq
#endif /* _ARTDAQ_DATABASE_FHICLJSON_FHICL_WRITER_H_ */
