#include "artdaq-database/Overlay/ovlChangeLog.h"
#include "artdaq-database/Overlay/ovlKeyValue.h"

namespace jsonliteral = artdaq::database::dataformats::literal;
namespace ovl = artdaq::database::overlay;
using namespace artdaq::database;
using namespace artdaq::database::overlay;
using namespace artdaq::database::result;
using result_t = artdaq::database::result_t;
using artdaq::database::sharedtypes::unwrap;

ovlChangeLog::ovlChangeLog(object_t::key_type const& key, value_t& changelog) : ovlStringKeyValue(key, changelog) {}

std::string& ovlChangeLog::buffer() { return string_value(); }

std::string const& ovlChangeLog::buffer() const { return string_value(); }

std::string& ovlChangeLog::buffer(std::string const& changelog) {
  buffer() = changelog;

  return buffer();
}

std::string& ovlChangeLog::append(std::string const& changelog) {
  buffer() += changelog;

  return buffer();
}

result_t ovlChangeLog::operator==(ovlChangeLog const& other) const {
  return ((useCompareMask() & DOCUMENT_COMPARE_MUTE_CHANGELOG) == DOCUMENT_COMPARE_MUTE_CHANGELOG)
             ? Success()
             : self() == other.self();
}
