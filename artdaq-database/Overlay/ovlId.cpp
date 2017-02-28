#include "artdaq-database/Overlay/ovlId.h"
#include "artdaq-database/Overlay/ovlKeyValue.h"

namespace jsonliteral = artdaq::database::dataformats::literal;
namespace ovl = artdaq::database::overlay;
using namespace artdaq::database;
using namespace artdaq::database::overlay;
using namespace artdaq::database::result;
using result_t = artdaq::database::result_t;
using artdaq::database::sharedtypes::unwrap;

ovlId::ovlId(object_t::key_type const& key, value_t& oid) : ovlKeyValue(key, oid), _initOK(init(oid)) {}

bool ovlId::init(value_t& parent) try {
  confirm(type(parent) == type_t::OBJECT);

  auto& obj = object_value();

  if (obj.count(jsonliteral::oid) == 0) obj[jsonliteral::oid] = generate_oid();

  confirm(obj.count(jsonliteral::oid) == 1);

  return true;
} catch (...) {
  confirm(false);
  throw;
}

bool ovlId::newId() try {
  auto& obj = object_value();
  obj[jsonliteral::oid] = generate_oid();
  confirm(obj.count(jsonliteral::oid) == 1);

  return true;
} catch (...) {
  confirm(false);
  throw;
}

std::string& ovlId::oid() { return value_as<std::string>(jsonliteral::oid); }

std::string& ovlId::oid(std::string const& id) {
  confirm(id.empty());

  oid() = id;
  return oid();
}

result_t ovlId::operator==(ovlId const& other) const {
  return ((useCompareMask() & DOCUMENT_COMPARE_MUTE_OUIDS) == DOCUMENT_COMPARE_MUTE_OUIDS) ? Success()
                                                                                           : self() == other.self();
}
