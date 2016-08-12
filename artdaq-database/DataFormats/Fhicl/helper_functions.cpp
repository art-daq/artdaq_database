#include "artdaq-database/DataFormats/Fhicl/helper_functions.h"
#include "artdaq-database/DataFormats/common.h"

namespace artdaq {
namespace database {
namespace fhicl {

namespace literal = artdaq::database::dataformats::literal;

bool isDouble(std::string const& str) {
  std::regex ex({literal::regex::parse_double});
  return std::regex_match(str, ex);
}

std::string unescape(std::string const& str) { return std::regex_replace(str, std::regex("\\\""), "\""); }

std::string tag_as_string(::fhicl::value_tag tag) {
  switch (tag) {
    default:
      return "undefined";
    case ::fhicl::UNKNOWN:
      return "unknown";
    case ::fhicl::NIL:
      return "nil";
    case ::fhicl::STRING:
      return "string";
    case ::fhicl::BOOL:
      return "bool";
    case ::fhicl::NUMBER:
      return "number";
    case ::fhicl::COMPLEX:
      return "complex";
    case ::fhicl::SEQUENCE:
      return "sequence";
    case ::fhicl::TABLE:
      return "table";
    case ::fhicl::TABLEID:
      return "tableid";
  }
}

::fhicl::value_tag string_as_tag(std::string name) {
  auto str = std::move(name);
  if (str == "nil")
    return ::fhicl::NIL;
  else if (str == "string")
    return ::fhicl::STRING;
  else if (str == "bool")
    return ::fhicl::BOOL;
  else if (str == "number")
    return ::fhicl::NUMBER;
  else if (str == "complex")
    return ::fhicl::COMPLEX;
  else if (str == "sequence")
    return ::fhicl::SEQUENCE;
  else if (str == "table")
    return ::fhicl::TABLE;
  else if (str == "tableid")
    return ::fhicl::TABLEID;

  throw ::fhicl::exception(::fhicl::parse_error, literal::data_node)
      << ("FHiCL atom type \"" + str + "\" is not implemented.");
}
}  // namespace fhicl
}  // namespace database
}  // artdaq