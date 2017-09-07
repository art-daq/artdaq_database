#include "artdaq-database/Overlay/ovlComment.h"
#include "artdaq-database/Overlay/ovlKeyValue.h"

namespace jsonliteral = artdaq::database::dataformats::literal;
namespace ovl = artdaq::database::overlay;
using namespace artdaq::database;
using namespace artdaq::database::overlay;
using namespace artdaq::database::result;
using result_t = artdaq::database::result_t;
using artdaq::database::sharedtypes::unwrap;

ovlComment::ovlComment(object_t::key_type const& key, value_t& comment) : ovlKeyValue(key, comment) {}

integer& ovlComment::linenum() { return value_as<integer>(jsonliteral::linenum); }

std::string& ovlComment::text() { return value_as<std::string>(jsonliteral::value); }

integer const& ovlComment::linenum() const { return value_as<integer>(jsonliteral::linenum); }

std::string const& ovlComment::text() const { return value_as<std::string>(jsonliteral::value); }

std::string ovlComment::to_string() const {
  std::ostringstream oss;
  oss << "{" << quoted_(jsonliteral::linenum) << ":" << linenum() << ",";
  oss << quoted_(jsonliteral::value) << ":" << quoted_(text()) << "}";

  return oss.str();
}

result_t ovlComment::operator==(ovlComment const& other) const { return self() == other.self(); }
