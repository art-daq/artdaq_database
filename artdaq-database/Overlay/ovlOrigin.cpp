#include "artdaq-database/Overlay/ovlOrigin.h"
#include "artdaq-database/Overlay/ovlKeyValue.h"
#include "artdaq-database/Overlay/ovlTimeStamp.h"

using namespace artdaq::database;
using namespace artdaq::database::overlay;
using namespace artdaq::database::result;
using result_t = artdaq::database::result_t;

ovlOrigin::ovlOrigin(object_t::key_type const& key, value_t& origin)
    : ovlKeyValue(key, origin),
      _initOK(init(origin)),
      _created(map_created(origin)),
      _rawdatalist{overlay<ovlRawDataList, array_t>(origin, jsonliteral::rawdatalist)} {}

std::string& ovlOrigin::format() { return value_as<std::string>(jsonliteral::format); }

std::string const& ovlOrigin::name() const { return value_as<std::string>(jsonliteral::name); }

std::string& ovlOrigin::name() { return value_as<std::string>(jsonliteral::name); }

std::string const& ovlOrigin::format() const { return value_as<std::string>(jsonliteral::format); }

std::string& ovlOrigin::source() { return value_as<std::string>(jsonliteral::source); }

std::string const& ovlOrigin::source() const { return value_as<std::string>(jsonliteral::source); }

std::string ovlOrigin::to_string() const {
  std::ostringstream oss;
  oss << "{" << quoted_(jsonliteral::origin) << ":{\n";
  oss << quoted_(jsonliteral::format) << ":" << quoted_(format()) << ",\n";
  oss << quoted_(jsonliteral::name) << ":" << quoted_(name()) << ",\n";
  oss << quoted_(jsonliteral::source) << ":" << quoted_(source()) << ",\n";
  oss << debrace(_created.to_string()) << ",\n";
  oss << debrace(_rawdatalist->to_string()) << "\n";
  oss << "}}";

  return oss.str();
}

bool ovlOrigin::init(value_t& parent) try {
  confirm(type(parent) == type_t::OBJECT);

  auto& obj = object_value();

  if (obj.count(jsonliteral::source) == 0) {
    obj[jsonliteral::source] = "template"s;
  }
  confirm(obj.count(jsonliteral::source) == 1);

  if (obj.count(jsonliteral::name) == 0) {
    obj[jsonliteral::name] = std::string{jsonliteral::notprovided};
  }
  confirm(obj.count(jsonliteral::name) == 1);

  if (obj.count(jsonliteral::format) == 0) {
    obj[jsonliteral::format] = "json"s;
  }
  confirm(obj.count(jsonliteral::format) == 1);

  if (obj.count(jsonliteral::created) == 0) {
    obj[jsonliteral::created] = timestamp();
  }
  confirm(obj.count(jsonliteral::created) == 1);

  if (obj.count(jsonliteral::rawdatalist) == 0) {
    obj[jsonliteral::rawdatalist] = array_t{};
  }
  confirm(obj.count(jsonliteral::rawdatalist) == 1);

  return true;
} catch (...) {
  confirm(false);
  throw;
}

result_t ovlOrigin::operator==(ovlOrigin const& other) const {
  if ((useCompareMask() & DOCUMENT_COMPARE_MUTE_ORIGIN) == DOCUMENT_COMPARE_MUTE_ORIGIN) {
    return Success();
  }

  std::ostringstream oss;
  oss << "\nOrigin nodes disagree.";
  auto noerror_pos = oss.tellp();

  if (format() != other.format()) {
    oss << "\n  Formats are different: self,other=" << quoted_(format()) << "," << quoted_(other.format());
  }

  if (name() != other.name()) {
    oss << "\n  File names are different: self,other=" << quoted_(name()) << "," << quoted_(other.name());
  }

  if (source() != other.source()) {
    oss << "\n  Sources are different: self,other=" << quoted_(source()) << "," << quoted_(other.source());
  }

  auto result = _created == other._created;

  if (!result.first) {
    oss << "\n  Timestamps are different: self,other=" << quoted_(_created.timestamp()) << ","
        << quoted_(other._created.timestamp());
  }

  result = *_rawdatalist == *other._rawdatalist;

  if (!result.first) {
    oss << result.second;
  }

  if (oss.tellp() == noerror_pos) {
    return Success();
  }

  oss << "\n  Debug info:";
  oss << "\n  Self  value: " << to_string();
  oss << "\n  Other value: " << other.to_string();

  return Failure(oss);
}
ovlTimeStamp ovlOrigin::map_created(value_t& parent) {
  confirm(type(parent) == type_t::OBJECT);

  auto& obj = object_value();

  confirm(obj.count(jsonliteral::created) == 1);

  return ovlTimeStamp(jsonliteral::created, obj.at(jsonliteral::created));
}
