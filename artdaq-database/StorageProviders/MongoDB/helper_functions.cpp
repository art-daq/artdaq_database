#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/DataFormats/Json/json_common.h"
#include "artdaq-database/SharedCommon/sharedcommon_common.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "PRVDR:MongoDB_C"

namespace artdaq {
namespace database {
namespace sharedtypes {
using artdaq::database::sharedtypes::unwrapper;

template <>
template <typename T>
T& unwrapper<jsn::value_t>::value_as() {
  return boost::get<T>(any);
}
  
}

namespace mongo {  

using namespace artdaq::database;
namespace db = artdaq::database;
using artdaq::database::basictypes::JsonData;
using artdaq::database::json::object_t;
using artdaq::database::json::value_t;
using artdaq::database::json::JsonReader;
using artdaq::database::json::JsonWriter;
using artdaq::database::sharedtypes::unwrap;

JsonData rewrite_query_with_regex(JsonData const &query,
                                  std::vector<std::string> const &fields) {
  if (fields.empty())
    return query;

  object_t query_ast;

  if (!JsonReader().read(query.json_buffer, query_ast))
    throw runtime_error("MongoDB")
        << "Failed to create AST in rewrite_query_with_regex()";

  if (query_ast.empty())
    return query;

  for (auto const &field : fields) {        
    if(query_ast.count(field)==0)
      continue;
    
    auto value =  unwrap(query_ast.at(field)).value_as<std::string>();
	    
    if (value.back() != '*')
      continue;

    query_ast[field] = object_t{};

    auto &regex_value = unwrap(query_ast).value_as<object_t>(field);

    if (value == "*") {
      regex_value["$regex"] = std::string(".*");
      continue;
    }
    
    value.pop_back();

    regex_value["$regex"] = std::string("^") + value + ".*";
  }

  auto buffer = std::string{};

  if (!JsonWriter().write(query_ast, buffer))
    throw runtime_error("MongoDB")
        << "Failed to write AST in rewrite_query_with_regex()";

  return {buffer};
}

} // namespace mongo
} // namespace database
} // namespace artdaq
