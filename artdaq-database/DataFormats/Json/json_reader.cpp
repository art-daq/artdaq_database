#include "artdaq-database/DataFormats/Json/json_reader.h"
#include "artdaq-database/DataFormats/common.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "json_reader.cpp"

using namespace boost::spirit;
using namespace artdaq::database;

using artdaq::database::json::JsonReader;
using artdaq::database::json::object_t;

bool JsonReader::read(std::string const& in, object_t& ast) {
  confirm(!in.empty());
  confirm(ast.empty());

  TLOG(20) << "read() begin";
  
   if(in.size()>512){   
      TLOG(21) << "heavy read() " << debug::getStackTrace();
   }

  TLOG(22) << "read() in=<" << in << ">";

  auto result = bool(false);
  object_t buffer;

  json_parser_grammar<std::string::const_iterator> grammar;

  std::string::const_iterator start = in.begin();

  result = qi::phrase_parse(start, in.end(), grammar, ascii::space, buffer);

  if (result) {
    ast.swap(buffer);
  }

  TLOG(23) << "write() end ";
  
  return result;
}

std::pair<bool, std::string> artdaq::database::json::compare_json_objects(std::string const& first,
                                                                          std::string const& second) {
  confirm(!first.empty());
  confirm(!second.empty());

  auto firstAST = object_t{};
  auto secondAST = object_t{};

  if (!JsonReader{}.read(first, firstAST)) {
    return std::make_pair(false, "Unable to read first Json buffer");
  }

  if (!JsonReader{}.read(second, secondAST)) {
    return std::make_pair(false, "Unable to read second Json buffer");
  }

  return firstAST == secondAST;
}