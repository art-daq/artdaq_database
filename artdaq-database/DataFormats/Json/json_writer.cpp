#include "artdaq-database/DataFormats/Json/json_writer.h"
#include "artdaq-database/DataFormats/common.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "json_writer.cpp"

using namespace boost::spirit;
using namespace artdaq::database;

using artdaq::database::json::JsonWriter;
using artdaq::database::json::object_t;

bool JsonWriter::write(object_t const& ast, std::string& out) {
  confirm(out.empty());
  confirm(!ast.empty());
  
  TLOG(21) << "write() begin ";

  auto result = bool(false);
  auto buffer = std::string();
  buffer.reserve(10000);
  
  auto sink = std::back_insert_iterator<std::string>(buffer);

  json_generator_grammar<decltype(sink)> grammar;

  result = karma::generate(sink, grammar, ast);

  // result = true;

  if (result) {
    out.swap(buffer);
  }
  TLOG(22) << "write() out=<" << out << ">";
  TLOG(23) << "write() end ";
  return result;
}
