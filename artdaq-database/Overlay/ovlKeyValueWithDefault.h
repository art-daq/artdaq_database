#ifndef _ARTDAQ_DATABASE_OVERLAY_OVLKEYVALUEWDEFAULT_H_
#define _ARTDAQ_DATABASE_OVERLAY_OVLKEYVALUEWDEFAULT_H_

#include "artdaq-database/Overlay/common.h"
#include "artdaq-database/Overlay/ovlKeyValue.h"

namespace artdaq {
namespace database {
namespace overlay {
namespace jsonliteral = artdaq::database::dataformats::literal;
using namespace artdaq::database::result;

template <int mask>
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

template <int mask>
ovlKeyValueWithDefault<mask>::ovlKeyValueWithDefault(object_t::key_type const& key, value_t& value)
    : ovlKeyValue(key, value), _initOK(init(value)) {}

template <int mask>
bool ovlKeyValueWithDefault<mask>::init(value_t& parent) try {
  confirm(type(parent) == type_t::OBJECT);

  auto& obj = object_value();

  if (obj.count(key()) == 0) obj[key()] = object_t{};
  confirm(obj.count(key()) == 1);

  return true;
} catch (...) {
  confirm(false);
  throw;
}

template <int mask>
result_t ovlKeyValueWithDefault<mask>::operator==(ovlKeyValueWithDefault const& other) const {
  return ((useCompareMask() & mask) == mask) ? Success() : self() == other.self();
}

}  // namespace overlay
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_OVERLAY_OVLKEYVALUEWDEFAULT_H_ */