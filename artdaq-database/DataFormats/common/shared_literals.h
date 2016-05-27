#ifndef _ARTDAQ_DATABASE_SHAREDLITERALS_H_
#define _ARTDAQ_DATABASE_SHAREDLITERALS_H_

namespace artdaq {
namespace database {
namespace dataformats {
namespace literal {
constexpr auto document_node = "document";
constexpr auto data_node = "data";
constexpr auto prolog_node = "prolog";
constexpr auto main_node = "main";
constexpr auto comments_node = "comments";
constexpr auto origin_node = "origin";
constexpr auto includes_node = "includes";
constexpr auto timestamp = "timestamp";
constexpr auto source = "source";
constexpr auto format = "format";
constexpr auto file = "file";
constexpr auto value = "value";
constexpr auto string = "string";
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
constexpr auto metadata_node = "metadata";
constexpr auto changelog = "changelog";
constexpr auto object = "object";
constexpr auto table = "table";
constexpr auto array = "array";
constexpr auto sequence = "sequence";

constexpr auto gui_data_node = "guidata";
constexpr auto converted = "converted";

namespace regex {
constexpr auto parse_double = "-?\\d*\\.?\\d+e[+-]?\\d+";
constexpr auto parse_include = "(#include)";
}
}

}  // namespace dataformats
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_SHAREDLITERALS_H_ */
