#include "artdaq-database/Overlay/ovlBookkeeping.h"
#include "artdaq-database/Overlay/ovlKeyValue.h"

namespace jsonliteral = artdaq::database::dataformats::literal;
namespace ovl = artdaq::database::overlay;
using namespace artdaq::database;
using namespace artdaq::database::overlay;
using namespace artdaq::database::result;
using result_t = artdaq::database::result_t;
using artdaq::database::sharedtypes::unwrap;

ovlBookkeeping::ovlBookkeeping(object_t::key_type const& key, value_t& bookkeeping)
    : ovlKeyValue(key, bookkeeping),
      _initOK(init(bookkeeping)),
      _updates(make_updates(bookkeeping)),
      _created(map_created(bookkeeping)) {}

bool& ovlBookkeeping::isReadonly() { return value_as<bool>(jsonliteral::isreadonly); }

bool const& ovlBookkeeping::isReadonly() const { return value_as<bool>(jsonliteral::isreadonly); }

bool& ovlBookkeeping::markReadonly(bool const& state) {
  value_as<bool>(jsonliteral::isreadonly) = state;

  return isReadonly();
}

bool& ovlBookkeeping::isDeleted() { return value_as<bool>(jsonliteral::isdeleted); }

bool const& ovlBookkeeping::isDeleted() const { return value_as<bool>(jsonliteral::isdeleted); }

bool& ovlBookkeeping::markDeleted(bool const& state) {
  value_as<bool>(jsonliteral::isdeleted) = state;

  return isDeleted();
}

std::string ovlBookkeeping::to_string() const {
  std::ostringstream oss;
  oss << "{" << quoted_(jsonliteral::bookkeeping) << ": {\n";
  oss << quoted_(jsonliteral::isreadonly) << ":" << bool_(isReadonly()) << ",\n";
  oss << quoted_(jsonliteral::isdeleted) << ":" << bool_(isDeleted()) << ",\n";
  oss << debrace(_created.to_string()) << ",\n";
  oss << quoted_(jsonliteral::updates) << ": [";

  for (auto const& update : _updates) oss << "\n" << update.to_string() << ",";

  if (!_updates.empty()) oss.seekp(-1, oss.cur);

  oss << "\n]\n}\n}";

  return oss.str();
}

ovlBookkeeping::updates_t ovlBookkeeping::make_updates(value_t& value) {
  confirm(type(value) == type_t::OBJECT);
  auto& obj = object_value();

  if (obj.count(jsonliteral::updates) == 0) obj[jsonliteral::updates] = array_t{};

  confirm(obj.count(jsonliteral::updates) == 1);

  auto& updates = unwrap(obj).value_as<array_t>(jsonliteral::updates);

  auto returnValue = ovlBookkeeping::updates_t{};

  for (auto& update : updates) returnValue.push_back({jsonliteral::update, update});

  return returnValue;
}

ovlTimeStamp ovlBookkeeping::map_created(value_t& value) {
  confirm(type(value) == type_t::OBJECT);
  auto& obj = object_value();

  if (obj.count(jsonliteral::created) == 0) obj[jsonliteral::created] = timestamp();

  confirm(obj.count(jsonliteral::created) == 1);

  return ovlTimeStamp(jsonliteral::created, obj.at(jsonliteral::created));
}

bool ovlBookkeeping::init(value_t& parent) try {
  confirm(type(parent) == type_t::OBJECT);

  auto& obj = object_value();

  if (obj.count(jsonliteral::isdeleted) == 0) obj[jsonliteral::isdeleted] = false;

  if (obj.count(jsonliteral::isreadonly) == 0) obj[jsonliteral::isreadonly] = false;

  return true;
} catch (...) {
  confirm(false);
  throw;
}

result_t ovlBookkeeping::operator==(ovlBookkeeping const& other) const {
  if ((useCompareMask() & DOCUMENT_COMPARE_MUTE_BOOKKEEPING) == DOCUMENT_COMPARE_MUTE_BOOKKEEPING) return Success();

  std::ostringstream oss;
  oss << "\nBookkeeping nodes disagree.";
  auto noerror_pos = oss.tellp();

  if (isDeleted() != other.isDeleted())
    oss << "\n  isdeleted flags are different: self,other=" << bool_(isDeleted()) << "," << bool_(other.isDeleted());

  if (isReadonly() != other.isReadonly())
    oss << "\n  isreadonly flags are different: self,other=" << bool_(isReadonly()) << "," << bool_(other.isReadonly());

  auto result = _created == other._created;

  if (!result.first)
    oss << "\n  Timestamps are different: self,other=" << quoted_(_created.timestamp()) << ","
        << quoted_(other._created.timestamp());

  if (oss.tellp() == noerror_pos && (useCompareMask() & DOCUMENT_COMPARE_MUTE_UPDATES) == DOCUMENT_COMPARE_MUTE_UPDATES)
    return Success();

  if (_updates.size() != other._updates.size())
    oss << "\n  Record update histories have different size: self,other=" << _updates.size() << ","
        << other._updates.size();

  if (oss.tellp() == noerror_pos && std::equal(_updates.cbegin(), _updates.end(), other._updates.cbegin(),
                                               [&oss](auto const& first, auto const& second) -> bool {
                                                 auto result = first == second;
                                                 if (result.first) return true;
                                                 oss << "\n  Record update histories are different: self,other="
                                                     << first.to_string() << "," << second.to_string();
                                                 return false;
                                               }))
    return Success();

  oss << "\n  Debug info:";
  oss << "\n  Self  value:\n" << to_string();
  oss << "\n  Other value:\n" << other.to_string();

  return Failure(oss);
}