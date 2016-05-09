#include "artdaq-database/DataFormats/Json/json_reader.h"
#include "artdaq-database/DataFormats/common.h"
#include "artdaq-database/DataFormats/common/shared_literals.h"

using namespace boost::spirit;
using namespace artdaq::database;

using artdaq::database::json::object_t;
using artdaq::database::json::JsonReader;

bool JsonReader::read(std::string const& in, object_t& ast) {
  assert(!in.empty());
  assert(ast.empty());

  auto result = bool(false);
  object_t buffer;

  json_parser_grammar<std::string::const_iterator> grammar;

  std::string::const_iterator start = in.begin();

  result = qi::phrase_parse(start, in.end(), grammar, ascii::space, buffer);

  if (result) ast.swap(buffer);

  return result;
}
