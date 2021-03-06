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
constexpr auto entity = "entity";
constexpr auto configurations = "configurations";
constexpr auto id = "_id";
constexpr auto oid = "$oid";
constexpr auto in = "$in";
constexpr auto includes = "includes";
constexpr auto timestamp = "timestamp";
constexpr auto source = "source";
constexpr auto format = "format";
constexpr auto file = "file";
constexpr auto value = "value";

constexpr auto nil = "nil";
constexpr auto string = "string";
constexpr auto boolean = "bool";
constexpr auto complex = "complex";
constexpr auto number = "number";
constexpr auto tableid = "tableid";

constexpr auto type = "type";

constexpr auto empty = "empty";
constexpr auto comment = "comment";
constexpr auto annotation = "annotation";
constexpr auto protection = "protection";

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
constexpr auto document = "document";
constexpr auto configurationtype = "configtype";
constexpr auto gui_data = "guidata";
constexpr auto converted = "converted";
constexpr auto notprovided = "notprovided";

constexpr auto filter = "filter";
constexpr auto ouid = "ouid";

constexpr auto schema = "schema";

constexpr auto string_singlequoted = "string_singlequoted";
constexpr auto string_doublequoted = "string_doublequoted";
constexpr auto string_unquoted = "string_unquoted";

constexpr auto origin_format = "origin.format";
constexpr auto document_data = "document.data";

constexpr auto empty_json = "{}";
namespace regex {
constexpr auto parse_decimal = "-?\\d*\\.?\\d+e[+-]?\\d+";
constexpr auto parse_include = "(#include)";
}  // namespace regex
}  // namespace literal

}  // namespace dataformats
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_SHAREDLITERALS_H_ */
