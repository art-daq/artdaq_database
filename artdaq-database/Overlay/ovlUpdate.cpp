
#include "artdaq-database/Overlay/ovlUpdate.h"

namespace jsonliteral = artdaq::database::dataformats::literal;
namespace ovl = artdaq::database::overlay;
using namespace artdaq::database;
using namespace artdaq::database::overlay;
using namespace artdaq::database::result;
using result_t = artdaq::database::result_t;
using artdaq::database::sharedtypes::unwrap;

ovlUpdate::ovlUpdate(object_t::key_type const& key, value_t& update)
    : ovlKeyValue(key, update), _timestamp(map_timestamp(update)), _what(map_what(update)) {}

std::string& ovlUpdate::name() { return value_as<std::string>(jsonliteral::event); }

std::string const& ovlUpdate::name() const { return value_as<std::string>(jsonliteral::event); }

std::string& ovlUpdate::timestamp() { return _timestamp.timestamp(); }

std::string const& ovlUpdate::timestamp() const { return _timestamp.timestamp(); }

ovlUpdateEntry& ovlUpdate::what() { return _what; }

ovlUpdateEntry const& ovlUpdate::what() const { return _what; }

std::string ovlUpdate::to_string() const {
  std::ostringstream oss;

  oss << "{";
  oss << quoted_(jsonliteral::event) << ":" << quoted_(name()) << ",";
  oss << debrace(_timestamp.to_string()) << ",";
  oss << debrace(_what.to_string());
  oss << "}";

  return oss.str();
}

ovlTimeStamp ovlUpdate::map_timestamp(value_t& value) {
  confirm(type(value) == type_t::OBJECT);

  auto& obj = object_value();

  confirm(obj.count(jsonliteral::timestamp) == 1);

  return ovlTimeStamp(jsonliteral::timestamp, obj.at(jsonliteral::timestamp));
}

ovlUpdateEntry ovlUpdate::map_what(value_t& value) {
  confirm(type(value) == type_t::OBJECT);
  auto& obj = object_value();

  confirm(obj.count(jsonliteral::value) == 1);

  return ovlUpdateEntry(jsonliteral::value, obj.at(jsonliteral::value));
}

result_t ovlUpdate::operator==(ovlUpdate const& other) const {
  std::ostringstream oss;
  oss << "\nUpdate events disagree.";
  auto noerror_pos = oss.tellp();

  auto result = _timestamp == other._timestamp;

  if (!result.first)
    oss << "\n  Timestamps are different: self,other=" << quoted_(_timestamp.timestamp()) << ","
        << quoted_(other._timestamp.timestamp());

  if (name() != other.name())
    oss << "\n  Events are different: self,other=" << quoted_(name()) << "," << quoted_(other.name());

  if ((useCompareMask() & DOCUMENT_COMPARE_MUTE_TIMESTAMPS) == DOCUMENT_COMPARE_MUTE_TIMESTAMPS) {
    auto const& name = _what.value_as<std::string>(jsonliteral::name);
    auto const& otherName = other._what.value_as<std::string>(jsonliteral::name);

    if (name != otherName)
      oss << "\n  Event names are different: self,other=" << quoted_(name) << "," << quoted_(otherName);
  } else {
    result = _what == other._what;

    if (!result.first)
      oss << "\n  Event data are different: self,other=" << what().to_string() << "," << other.what().to_string();
  }

  if (oss.tellp() == noerror_pos) return Success();

  return Failure(oss);
}