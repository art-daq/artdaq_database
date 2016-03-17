#include "artdaq-database/FhiclJson/common.h"
#include "artdaq-database/FhiclJson/json_writer.h"
#include "artdaq-database/FhiclJson/shared_literals.h"

using namespace boost::spirit;
using namespace artdaq::database;

using artdaq::database::json::object_t;
using artdaq::database::json::JsonWriter;

bool JsonWriter::write(object_t const& ast, std::string& out) {
  assert(out.empty());
  assert(!ast.empty());

  auto result = bool(false);
  auto buffer = std::string();

  auto sink = std::back_insert_iterator<std::string>(buffer);

  json_generator_grammar<decltype(sink)> grammar;

  result = karma::generate(sink, grammar, ast);

  result = true;

  if (result) out.swap(buffer);

  return result;
}
