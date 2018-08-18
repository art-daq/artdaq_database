#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/DataFormats/Json/json_common.h"
#include "artdaq-database/DataFormats/Json/json_types_impl.h"
#include "artdaq-database/SharedCommon/sharedcommon_common.h"
#include "artdaq-database/JsonDocument/JSONDocument.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "helper_functions.cpp"

namespace artdaq {
namespace database {
namespace mongo {

using namespace artdaq::database;
using artdaq::database::docrecord::JSONDocument;
using artdaq::database::json::JsonReader;
using artdaq::database::json::JsonWriter;
using artdaq::database::json::object_t;
using artdaq::database::json::value_t;
using artdaq::database::sharedtypes::unwrap;

JSONDocument rewrite_query_with_regex(JSONDocument const& query, std::vector<std::string> const& fields) {
  if (fields.empty()) {
    return query;
  }

  object_t query_ast;

  if (!JsonReader().read(query, query_ast)) {
    throw runtime_error("MongoDB") << "Failed to create AST in rewrite_query_with_regex()";
  }

  if (query_ast.empty()) {
    return query;
  }

  for (auto const& field : fields) {
    if (query_ast.count(field) == 0) {
      continue;
    }

    auto value = unwrap(query_ast.at(field)).value_as<std::string>();

    if (value.back() != '*') {
      continue;
    }

    query_ast[field] = object_t{};

    auto& regex_value = unwrap(query_ast).value_as<object_t>(field);

    if (value == "*") {
      regex_value["$regex"] = std::string(".*");
      continue;
    }

    value.pop_back();

    regex_value["$regex"] = std::string("^") + value + ".*";
  }

  auto buffer = std::string{};

  if (!JsonWriter().write(query_ast, buffer)) {
    throw runtime_error("MongoDB") << "Failed to write AST in rewrite_query_with_regex()";
  }

  return {buffer};
}

}  // namespace mongo
}  // namespace database
}  // namespace artdaq
