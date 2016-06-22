#include "artdaq-database/JsonDocument/JSONDocument.h"

#include "artdaq-database/JsonDocument/common.h"

#include <bsoncxx/json.hpp>

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "JSNU:Document_C"

namespace artdaq {
namespace database {
namespace mongo {
std::string to_json(bsoncxx::document::view view);
}  // namespace mongo
}  // namespace database
}  // namespace artdaq


namespace adj= artdaq::database::jsonutils;
  
bool adj::isValidJson(std::string const& json_buffer [[gnu::unused]]) {
  auto document = bsoncxx::from_json(json_buffer);

  if (!document) return false;

  return true;
}

std::string adj::filterJson(std::string const& json) {
  assert(!json.empty());

  TRACE_(10, "filterJson() json=<" << json << ">");

  auto document = bsoncxx::from_json(json);

  if (!document) {
    TRACE_(10, "filterJson()"
                   << "Invalid JSON document.");

    throw cet::exception("JSONDocument_utils") << "Invalid JSON document; json=" << json;
  }

  return  artdaq::database::mongo::to_json(document->view());
}
