#ifndef _ARTDAQ_DATABASE_DATAFORMATS_JSON_READER_H_
#define _ARTDAQ_DATABASE_DATAFORMATS_JSON_READER_H_

#include "artdaq-database/DataFormats/Json/json_types.h"
#include "artdaq-database/DataFormats/common.h"

#include <boost/spirit/include/qi.hpp>

namespace artdaq {
namespace database {
namespace json {

using namespace boost::spirit;

template <typename Iter>
struct json_parser_grammar : qi::grammar<Iter, object_t(), ascii::space_type> {
  json_parser_grammar() : json_parser_grammar::base_type(start) {
    escape_rule = ascii::char_('\\') >> ascii::char_("\\\"bfnrt");

    text_string = +(escape_rule | ~ascii::char_('"'));

    quoted_string = qi::lexeme['"' >> *text_string >> '"'];

    object_rule = '{' >> -(data_rule % ',') >> '}';

    key_rule = quoted_string;

    data_rule = key_rule >> ':' >> value_rule;

    array_rule = '[' >> -(value_rule % ',') >> ']';

    value_rule = object_rule | array_rule | quoted_string | (qi::int_ >> !qi::lit('.')) | qi::double_ | qi::bool_;

    start = object_rule;

    BOOST_SPIRIT_DEBUG_NODE(key_rule);
    BOOST_SPIRIT_DEBUG_NODE(value_rule);
    BOOST_SPIRIT_DEBUG_NODE(data_rule);
    BOOST_SPIRIT_DEBUG_NODE(object_rule);
    BOOST_SPIRIT_DEBUG_NODE(array_rule);
    BOOST_SPIRIT_DEBUG_NODE(start);
  }

  qi::rule<Iter, std::string(), ascii::space_type> quoted_string;
  qi::rule<Iter, value_t(), ascii::space_type> value_rule;
  qi::rule<Iter, key_t(), ascii::space_type> key_rule;
  qi::rule<Iter, data_t(), ascii::space_type> data_rule;
  qi::rule<Iter, object_t(), ascii::space_type> start, object_rule;
  qi::rule<Iter, array_t(), ascii::space_type> array_rule;
  qi::rule<Iter, std::string()> text_string;
  qi::rule<Iter, std::string()> escape_rule;
};

struct JsonReader final {
  bool read(std::string const&, object_t&);
};

std::pair<bool, std::string> compare_json_objects(std::string const&, std::string const&);
}  // namespace json
}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_DATAFORMATS_JSON_READER_H_ */
