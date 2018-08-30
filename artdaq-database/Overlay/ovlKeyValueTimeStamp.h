#ifndef _ARTDAQ_DATABASE_OVERLAY_OVLKEYVALUETS_H_
#define _ARTDAQ_DATABASE_OVERLAY_OVLKEYVALUETS_H_

#include "artdaq-database/Overlay/common.h"
#include "artdaq-database/Overlay/ovlKeyValue.h"
#include "artdaq-database/Overlay/ovlTimeStamp.h"

namespace artdaq {
namespace database {
namespace overlay {
namespace jsonliteral = artdaq::database::dataformats::literal;
using namespace artdaq::database::result;

template <std::uint32_t mask, bool A = true, bool R = false>
class ovlKeyValueTimeStamp final : public ovlKeyValue {
 public:
  ovlKeyValueTimeStamp(object_t::key_type const& /*key*/, value_t& /*value*/);

  // defaults
  ovlKeyValueTimeStamp(ovlKeyValueTimeStamp&&) = default;
  ~ovlKeyValueTimeStamp() = default;

  // accessors
  std::string& name();
  std::string const& name() const;
  std::string& name(std::string const& /*name*/);

  std::string& assigned();
  std::string& removed();

  // overrides
  std::string to_string() const override;

  // ops
  result_t operator==(ovlKeyValueTimeStamp const&) const;

 private:
  ovlTimeStampUPtr_t map_assigned(value_t& /*value*/);
  ovlTimeStampUPtr_t map_removed(value_t& /*value*/);
  bool init(value_t& /*parent*/);

 private:
  bool _initOK;
  ovlTimeStampUPtr_t _assigned;
  ovlTimeStampUPtr_t _removed;
};

template <std::uint32_t mask, bool A, bool R>
ovlKeyValueTimeStamp<mask, A, R>::ovlKeyValueTimeStamp(object_t::key_type const& key, value_t& value)
    : ovlKeyValue(key, value), _initOK(init(value)), _assigned(map_assigned(value)), _removed(map_assigned(value)) {}

template <std::uint32_t mask, bool A, bool R>
bool ovlKeyValueTimeStamp<mask, A, R>::init(value_t& parent) try {
  confirm(type(parent) == type_t::OBJECT);

  auto& obj = object_value();

  confirm(obj.count(jsonliteral::name) == 1);

  if (A) {
    if (obj.count(jsonliteral::assigned) == 0) obj[jsonliteral::assigned] = timestamp();
    confirm(obj.count(jsonliteral::assigned) == 1);
  }

  if (R) {
    if (obj.count(jsonliteral::removed) == 0) obj[jsonliteral::removed] = timestamp();
    confirm(obj.count(jsonliteral::removed) == 1);
  }

  return true;
} catch (...) {
  confirm(false);
  throw;
}

template <std::uint32_t mask, bool A, bool R>
std::string& ovlKeyValueTimeStamp<mask, A, R>::name() {
  return value_as<std::string>(jsonliteral::name);
}

template <std::uint32_t mask, bool A, bool R>
std::string const& ovlKeyValueTimeStamp<mask, A, R>::name() const {
  return value_as<std::string>(jsonliteral::name);
}

template <std::uint32_t mask, bool A, bool R>
std::string& ovlKeyValueTimeStamp<mask, A, R>::name(std::string const& name) {
  confirm(!name.empty());

  auto& returnValue = value_as<std::string>(jsonliteral::name);

  returnValue = name;

  return returnValue;
}

template <std::uint32_t mask, bool A, bool R>
std::string& ovlKeyValueTimeStamp<mask, A, R>::assigned() {
  confirm(A);

  return _assigned->timestamp();
}

template <std::uint32_t mask, bool A, bool R>
std::string& ovlKeyValueTimeStamp<mask, A, R>::removed() {
  confirm(R);

  return _removed->timestamp();
}

template <std::uint32_t mask, bool A, bool R>
std::string ovlKeyValueTimeStamp<mask, A, R>::to_string() const {
  std::ostringstream oss;
  oss << "{";
  oss << quoted_(jsonliteral::name) << ":" << quoted_(name());

  if (A) oss << "," << debrace(_assigned->to_string());
  if (R) oss << "," << debrace(_removed->to_string());

  oss << "}";

  return oss.str();
}

template <std::uint32_t mask, bool A, bool R>
ovlTimeStampUPtr_t ovlKeyValueTimeStamp<mask, A, R>::map_assigned(value_t& value) {
  confirm(type(value) == type_t::OBJECT);

  if (!A) return std::unique_ptr<ovlTimeStamp>(nullptr);

  auto& obj = object_value();

  confirm(obj.count(jsonliteral::assigned) == 1);

  return std::make_unique<ovlTimeStamp>(jsonliteral::assigned, obj.at(jsonliteral::assigned));
}

template <std::uint32_t mask, bool A, bool R>
ovlTimeStampUPtr_t ovlKeyValueTimeStamp<mask, A, R>::map_removed(value_t& value) {
  confirm(type(value) == type_t::OBJECT);

  if (!R) return std::unique_ptr<ovlTimeStamp>(nullptr);

  auto& obj = object_value();

  confirm(obj.count(jsonliteral::removed) == 1);

  return std::make_unique<ovlTimeStamp>(jsonliteral::removed, obj.at(jsonliteral::removed));
}

template <std::uint32_t mask, bool A, bool R>
result_t ovlKeyValueTimeStamp<mask, A, R>::operator==(ovlKeyValueTimeStamp const& other) const {
  std::ostringstream oss;
  oss << "\n" << key() << " records disagree.";

  auto noerror_pos = oss.tellp();

  if (name() != other.name()) oss << "\n  " << key() << "names are different: self,other=" << quoted_(name()) << "," << quoted_(other.name());

  if (A) {
    auto result = *_assigned == *(other._assigned);
    if (!result.first) oss << "\n  Assigned timestamps are different:\n  " << result.second;
  }

  if (R) {
    auto result = *_removed == *(other._assigned);
    if (!result.first) oss << "\n  Assigned timestamps are different:\n  " << result.second;
  }

  if (oss.tellp() == noerror_pos) return Success();

  oss << "\n  Debug info:";
  oss << "\n  Self  value: " << to_string();
  oss << "\n  Other value: " << other.to_string();

  return Failure(oss);
}
}  // namespace overlay
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_OVERLAY_OVLKEYVALUETS_H_ */