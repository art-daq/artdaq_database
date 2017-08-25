#include "artdaq-database/DataFormats/Fhicl/helper_functions.h"
#include "artdaq-database/DataFormats/common.h"
#include "artdaq-database/DataFormats/shared_literals.h"

namespace artdaq {
namespace database {
namespace fhicl {

namespace literal = artdaq::database::dataformats::literal;

bool isDouble(std::string const& str) {
  std::regex ex({literal::regex::parse_decimal});
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
  else if (str == "string" || str == "string_singlequoted" || str == "string_doublequoted")
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

  throw ::fhicl::exception(::fhicl::parse_error, literal::data)
      << ("FHiCL atom type \"" + str + "\" is not implemented.");
}

std::string protection_as_string(::fhicl::Protection protection){
  switch (protection) {
    default:
      return "@none";
    case ::fhicl::Protection::PROTECT_IGNORE:
      return "@protect_ignore";
    case ::fhicl::Protection::PROTECT_ERROR:
      return "@protect_error";
    
    /*
    case ::fhicl::Protection::INITIAL:
      return "@initial";
    case ::fhicl::Protection::REPLACE:
      return "@replace";
    case ::fhicl::Protection::REPLACE_COMPAT:
      return "@replace_compat";
    case ::fhicl::Protection::ADD_OR_REPLACE_COMPAT:
      return "@add_or_replace_compat";
    */
  }
}

::fhicl::Protection string_as_protection(std::string name){
  auto str = std::move(name);
  if (str == "" || str =="@none")
    return ::fhicl::Protection::NONE;
  else if (str == "@protect_ignore")
    return ::fhicl::Protection::PROTECT_IGNORE;
  else if (str == "@protect_error")
    return ::fhicl::Protection::PROTECT_ERROR;
/*
  else if (str == "@initial")
    return ::fhicl::Protection::INITIAL;
  else if (str == "@replace")
    return ::fhicl::Protection::REPLACE;
  else if (str == "@replace_compat")
    return ::fhicl::Protection::REPLACE_COMPAT;
  else if (str == "@add_or_replace_compat")
    return ::fhicl::Protection::ADD_OR_REPLACE_COMPAT;
*/

  throw ::fhicl::exception(::fhicl::parse_error, literal::data)
      << ("FHiCL protection option \"" + str + "\" is not implemented.");
  
}

}  // namespace fhicl
}  // namespace database
}  // artdaq
