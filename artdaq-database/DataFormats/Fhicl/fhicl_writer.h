#ifndef _ARTDAQ_DATABASE_DATAFORMATS_FHICL_FHICL_WRITER_H_
#define _ARTDAQ_DATABASE_DATAFORMATS_FHICL_FHICL_WRITER_H_

#include "artdaq-database/DataFormats/Fhicl/fhicl_types.h"
#include "artdaq-database/DataFormats/Json/json_types.h"
#include "artdaq-database/DataFormats/common.h"

#include <boost/spirit/include/karma.hpp>

namespace artdaq {
namespace database {
namespace fhicl {

using namespace boost::spirit;

template <typename Iter>
struct fhicl_generator_grammar : karma::grammar<Iter, table_t()> {
  fhicl_generator_grammar() : fhicl_generator_grammar::base_type(start) {
    variant_value_rule =  string | karma::long_ | karma::double_ | karma::bool_ |table_rule | sequence_rule ;

    annotated_value_rule = variant_value_rule << -annotation;

    string = karma::string;

    comment = -(string << karma::eol);

    annotation = " " << string << karma::eol;

    commented_key_rule = -comment << string;

    annotated_atom_rule = commented_key_rule << ": " << annotated_value_rule;

    table_rule = "{" << karma::eol << *(annotated_atom_rule) << karma::eol << "}";

    toplevel_table = *(annotated_atom_rule);

    sequence_rule = "[" << -(annotated_value_rule % ", ") << "]";

    start = toplevel_table;
  }

  karma::rule<Iter, std::string()> string, comment, annotation;
  karma::rule<Iter, variant_value_t()> variant_value_rule;
  karma::rule<Iter, value_t()> annotated_value_rule;
  karma::rule<Iter, key_t()> commented_key_rule;
  karma::rule<Iter, atom_t()> annotated_atom_rule;
  karma::rule<Iter, table_t()> start, table_rule, toplevel_table;
  karma::rule<Iter, sequence_t()> sequence_rule;
};

using include_t = std::string;
using includes_t = std::vector<include_t>;

template <typename Iter>
struct fhicl_include_generator_grammar : karma::grammar<Iter, includes_t()> {
  fhicl_include_generator_grammar() : fhicl_include_generator_grammar::base_type(includes_rule) {
    include_rule = karma::string;
    includes_rule = ("#include \"" << include_rule << "\"") % karma::eol;
  }

  karma::rule<Iter, include_t()> include_rule;
  karma::rule<Iter, includes_t()> includes_rule;
};

struct FhiclWriter final {
  bool write_data(jsn::object_t const&, std::string&) __attribute__ ((visibility ("default"))) ;
};

namespace debug {
void FhiclWriter() __attribute__ ((visibility ("default"))) ;
}
}  // namespace fhicl
}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_DATAFORMATS_FHICL_FHICL_WRITER_H_ */
