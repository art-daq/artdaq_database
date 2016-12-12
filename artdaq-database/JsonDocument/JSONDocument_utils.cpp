#include "artdaq-database/JsonDocument/JSONDocument.h"

#include "artdaq-database/DataFormats/Json/json_common.h"
#include "artdaq-database/JsonDocument/common.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "JSNU:Document_C"

namespace adj = artdaq::database::jsonutils;
namespace jsn = artdaq::database::json;

bool adj::isValidJson(std::string const& json_buffer) {
  assert(!json_buffer.empty());

  jsn::object_t doc_ast1;

  if (!jsn::JsonReader{}.read(json_buffer, doc_ast1)) {
    TRACE_(10, "Failed reading JSON buffer: << " << json_buffer << ">>\n");
    return false;
  }

  return true;
}

std::string adj::filterJson(std::string const& json_buffer) {
  assert(!json_buffer.empty());

  TRACE_(10, "filterJson() json=<" << json_buffer << ">");

  try {
    jsn::object_t doc_ast1;
    if (!jsn::JsonReader{}.read(json_buffer, doc_ast1)) {
      TRACE_(10, "Failed reading JSON buffer: << " << json_buffer << ">>\n");
      throw cet::exception("JSONDocument_utils") << "Failed reading JSON buffer: << " << json_buffer << ">>\n";
    }

    if (doc_ast1.empty()) return json_buffer;

    auto jsonout = std::string();

    if (!jsn::JsonWriter{}.write(doc_ast1, jsonout)) {
      TRACE_(10, "Failed writing JSON AST: << " << json_buffer << ">>\n");
      throw cet::exception("JSONDocument_utils") << "Failed writing JSON buffer: << " << json_buffer << ">>\n";
    }
    return jsonout;
  } catch (std::exception const& e) {
    TRACE_(10, "filterJson()"
                   << "Caught exception:" << e.what());

    throw cet::exception("JSONDocument_utils") << "Caught exception:" << e.what();
  }
}
