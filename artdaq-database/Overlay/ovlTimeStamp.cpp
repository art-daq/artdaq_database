#include "artdaq-database/Overlay/ovlTimeStamp.h"

namespace jsonliteral = artdaq::database::dataformats::literal;
namespace ovl = artdaq::database::overlay;
using namespace artdaq::database;
using namespace artdaq::database::overlay;
using namespace artdaq::database::result;
using result_t = artdaq::database::result_t;
using artdaq::database::sharedtypes::unwrap;

ovlTimeStamp::ovlTimeStamp(object_t::key_type const& key, value_t& ts)
    : ovlKeyValue(key, (ts=confirm_iso8601_timestamp(unwrap(ts).value_as<std::string>()),ts)) {}

std::string& ovlTimeStamp::timestamp() { return string_value(); }

std::string const& ovlTimeStamp::timestamp() const { return string_value(); }

std::string& ovlTimeStamp::timestamp(std::string const& ts) {
  confirm(!ts.empty());
  timestamp() = ts;

  return timestamp();
}

std::string ovlTimeStamp::to_string() const {
  std::ostringstream oss;
  oss << "{" << quoted_(key()) << ":" << quoted_(timestamp()) << "}";

  return oss.str();
}

result_t ovlTimeStamp::operator==(ovlTimeStamp const& other) const {
  return ((useCompareMask() & DOCUMENT_COMPARE_MUTE_TIMESTAMPS) == DOCUMENT_COMPARE_MUTE_TIMESTAMPS)
             ? Success()
             : self() == other.self();
}
