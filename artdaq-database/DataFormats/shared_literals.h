#ifndef _ARTDAQ_DATABASE_SHAREDLITERALS_H_
#define _ARTDAQ_DATABASE_SHAREDLITERALS_H_

#include <string>

using namespace std::string_literals;

namespace artdaq {
namespace database {
namespace dataformats {
namespace literal {

constexpr auto dot = ".";

constexpr auto database_record = "record";
constexpr auto main = "main";
constexpr auto prolog = "prolog";
constexpr auto entities = "entities";
constexpr auto configurations = "configurations";
constexpr auto id = "_id";
constexpr auto oid = "_oid";
constexpr auto includes = "includes";
constexpr auto timestamp = "timestamp";
constexpr auto source = "source";
constexpr auto format = "format";
constexpr auto file = "file";
constexpr auto value = "value";
constexpr auto string = "string";
constexpr auto type = "type";
constexpr auto whitespace = " ";
constexpr auto empty = "empty";
constexpr auto comment = "comment";
constexpr auto annotation = "annotation";
constexpr auto children = "children";
constexpr auto name = "name";
constexpr auto values = "values";
constexpr auto unknown = "unknown";
constexpr auto include = "#include";
constexpr auto linenum = "linenum";
constexpr auto changelog = "changelog";
constexpr auto object = "object";
constexpr auto table = "table";
constexpr auto array = "array";
constexpr auto sequence = "sequence";
constexpr auto isdeleted = "isdeleted";
constexpr auto isreadonly = "isreadonly";
constexpr auto bookkeeping = "bookkeeping";
constexpr auto collection = "collection";
constexpr auto runs = "runs";
constexpr auto attachments = "attachments";
constexpr auto comments = "comments";
constexpr auto version = "version";
constexpr auto updates = "updates";
constexpr auto update = "update";
constexpr auto operation = "operation";
constexpr auto configuration = "configuration";
constexpr auto assigned = "assigned";
constexpr auto removed = "removed";
constexpr auto created = "created";
constexpr auto rawdatalist = "rawdata";
constexpr auto aliases = "aliases";
constexpr auto alias = "alias";
constexpr auto active = "active";
constexpr auto history = "history";
constexpr auto event = "event";
constexpr auto metadata = "metadata";
constexpr auto origin = "origin";
constexpr auto data = "data";
constexpr auto search = "search";
constexpr auto query = "query";
constexpr auto operations = "operations";
constexpr auto document="document";
constexpr auto configurationtype="configtype";
constexpr auto gui_data = "guidata";
constexpr auto converted = "converted";
constexpr auto notprovided = "notprovided";

namespace regex {
constexpr auto parse_double = "-?\\d*\\.?\\d+e[+-]?\\d+";
constexpr auto parse_include = "(#include)";
}
}

}  // namespace dataformats
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_SHAREDLITERALS_H_ */
