#ifndef _ARTDAQ_DATABASE_OVERLAY_OVLKEYVALUEWDEFAULT_H_
#define _ARTDAQ_DATABASE_OVERLAY_OVLKEYVALUEWDEFAULT_H_

#include "artdaq-database/Overlay/common.h"
#include "artdaq-database/Overlay/ovlKeyValue.h"

namespace artdaq {
namespace database {
namespace overlay {
namespace jsonliteral = artdaq::database::dataformats::literal;
using namespace artdaq::database::result;
using artdaq::database::sharedtypes::unwrap;

template <std::uint32_t mask>
class ovlKeyValueWithDefault : public ovlKeyValue {
 public:
  ovlKeyValueWithDefault(object_t::key_type const& /*key*/, value_t& /*object*/);

  // defaults
  ovlKeyValueWithDefault(ovlKeyValueWithDefault&&) = default;
  ~ovlKeyValueWithDefault() = default;

  // ops
  result_t operator==(ovlKeyValueWithDefault const&) const;

 private:
  bool init(value_t& /*parent*/);

 private:
  bool _initOK;
};

template <std::uint32_t mask>
ovlKeyValueWithDefault<mask>::ovlKeyValueWithDefault(object_t::key_type const& key, value_t& value) : ovlKeyValue(key, value), _initOK(init(value)) {}

template <std::uint32_t mask>
bool ovlKeyValueWithDefault<mask>::init(value_t& parent) try {
  confirm(type(parent) == type_t::OBJECT);

  auto& obj = unwrap(parent).value_as<object_t>();

  if (obj.count(key()) == 0) obj[key()] = object_t{};
  confirm(obj.count(key()) == 1);

  return true;
} catch (...) {
  confirm(false);
  throw;
}

template <std::uint32_t mask>
result_t ovlKeyValueWithDefault<mask>::operator==(ovlKeyValueWithDefault const& other) const {
  return ((useCompareMask() & mask) == mask) ? Success() : self() == other.self();
}

}  // namespace overlay
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_OVERLAY_OVLKEYVALUEWDEFAULT_H_ */