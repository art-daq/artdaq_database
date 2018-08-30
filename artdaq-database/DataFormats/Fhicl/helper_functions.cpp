#include "artdaq-database/DataFormats/Fhicl/helper_functions.h"
#include "artdaq-database/DataFormats/common.h"
#include "artdaq-database/DataFormats/shared_literals.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "helper_functions.cpp"

namespace artdaq {
namespace database {
namespace fhicl {

namespace literal = artdaq::database::dataformats::literal;

bool isDouble(std::string const& str) {
  std::regex ex(literal::regex::parse_decimal);
  return std::regex_match(str, ex);
}

std::string unescape(std::string const& str) { return std::regex_replace(str, std::regex("\\\""), "\""); }

std::string to_json_string(std::string const& str) {
  std::ostringstream oss;
  for (auto c : str) {
    switch (c) {
      case '"':
        oss << "\\\"";
        break;
      case '\\':
        oss << "\\\\";
        break;
      case '\b':
        oss << "\\b";
        break;
      case '\f':
        oss << "\\f";
        break;
      case '\n':
        oss << "\\n";
        break;
      case '\r':
        oss << "\\r";
        break;
      case '\t':
        oss << "\\t";
        break;
      default:
        oss << c;
    }
  }
  return oss.str();
}

std::string from_json_string(std::string const& str) {
  std::ostringstream oss;
  auto size = str.size();

  for (std::size_t pos = 0; pos < size; pos++) {
    /*if(str[pos] == '\''){
      oss << "\\\'";
      continue;
    }
    */

    if (str[pos] != '\\') {
      oss << str[pos];
      continue;
    }

    switch (str[++pos]) {
      case '\\':
        oss << '\\';
        break;
      case '"':
        oss << '"';
        break;
      case 'b':
        oss << '\b';
        break;
      case 'f':
        oss << '\f';
        break;
      case 'n':
        oss << '\n';
        break;
      case 'r':
        oss << '\r';
        break;
      case 't':
        oss << '\t';
        break;
    }
  }
  return oss.str();
}

std::string tag_as_string(::fhicl::value_tag tag) {
  switch (tag) {
    default:
      return literal::unknown;
    case ::fhicl::NIL:
      return literal::nil;
    case ::fhicl::STRING:
      return literal::string;
    case ::fhicl::BOOL:
      return literal::boolean;
    case ::fhicl::NUMBER:
      return literal::number;
    case ::fhicl::COMPLEX:
      return literal::complex;
    case ::fhicl::SEQUENCE:
      return literal::sequence;
    case ::fhicl::TABLE:
      return literal::table;
    case ::fhicl::TABLEID:
      return literal::tableid;
  }
}

::fhicl::value_tag string_as_tag(std::string name) {
  auto str = std::move(name);
  if (str == literal::nil) {
    return ::fhicl::NIL;
  }
  if (str == literal::string || str == literal::string_unquoted || str == literal::string_singlequoted || str == literal::string_doublequoted) {
    return ::fhicl::STRING;
  }
  if (str == literal::boolean) {
    return ::fhicl::BOOL;
  }
  if (str == literal::number) {
    return ::fhicl::NUMBER;
  }
  if (str == literal::complex) {
    return ::fhicl::COMPLEX;
  }
  if (str == literal::sequence) {
    return ::fhicl::SEQUENCE;
  } else if (str == literal::table) {
    return ::fhicl::TABLE;
  } else if (str == literal::tableid) {
    return ::fhicl::TABLEID;
  }

  throw ::fhicl::exception(::fhicl::parse_error, literal::data) << ("FHiCL atom type \"" + str + "\" is not implemented.");
}

std::string protection_as_string(::fhicl::Protection protection) {
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

::fhicl::Protection string_as_protection(std::string name) {
  auto str = std::move(name);
  if (str.empty() || str == "@none") {
    return ::fhicl::Protection::NONE;
  }
  if (str == "@protect_ignore") {
    return ::fhicl::Protection::PROTECT_IGNORE;
  }
  if (str == "@protect_error") {
    return ::fhicl::Protection::PROTECT_ERROR;
  }
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

  throw ::fhicl::exception(::fhicl::parse_error, literal::data) << ("FHiCL protection option \"" + str + "\" is not implemented.");
}

}  // namespace fhicl
}  // namespace database
}  // namespace artdaq
