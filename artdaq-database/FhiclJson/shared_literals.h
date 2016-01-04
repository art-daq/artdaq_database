#ifndef _ARTDAQ_DATABASE_SHAREDLITERALS_H_
#define _ARTDAQ_DATABASE_SHAREDLITERALS_H_

namespace literal
{
constexpr auto data_node = "data";
constexpr auto comments_node = "comments";
constexpr auto origin_node = "origin";
constexpr auto includes_node = "includes";
constexpr auto timestamp = "timestamp";
constexpr auto source = "source";
constexpr auto file = "file";
constexpr auto value = "value";
constexpr auto type = "type";
constexpr auto whitespace = " ";
constexpr auto empty = "";
constexpr auto comment = "comment";
constexpr auto annotation = "annotation";
constexpr auto children = "children";
constexpr auto name = "name";
constexpr auto values = "values";
constexpr auto unknown = "unknown";
constexpr auto include = "#include";
constexpr auto linenum = "linenum";

}

namespace regex
{
  constexpr auto parse_double ="-?\\d*\\.?\\d+e[+-]?\\d+";
  constexpr auto parse_include ="(#include)";  
}

#endif /* _ARTDAQ_DATABASE_SHAREDLITERALS_H_ */

