#ifndef _ARTDAQ_DATABASE_OVERLAY_OVLKEYVALUEWMASK_H_
#define _ARTDAQ_DATABASE_OVERLAY_OVLKEYVALUEWMASK_H_

#include "artdaq-database/Overlay/common.h"
#include "artdaq-database/Overlay/ovlKeyValue.h"

namespace artdaq {
namespace database {
namespace overlay {
namespace jsonliteral = artdaq::database::dataformats::literal;
using namespace artdaq::database::result;
using artdaq::database::sharedtypes::unwrap;

template <std::uint32_t mask>
class ovlKeyValueWithMask : public ovlKeyValue {
 public:
  ovlKeyValueWithMask(object_t::key_type const& /*key*/, value_t& /*object*/);

  // defaults
  ovlKeyValueWithMask(ovlKeyValueWithMask&&) = default;
  ~ovlKeyValueWithMask() = default;

  // ops
  result_t operator==(ovlKeyValueWithMask const&) const;
};

template <std::uint32_t mask>
ovlKeyValueWithMask<mask>::ovlKeyValueWithMask(object_t::key_type const& key, value_t& value)
    : ovlKeyValue(key, value) {}

template <std::uint32_t mask>
result_t ovlKeyValueWithMask<mask>::operator==(ovlKeyValueWithMask const& other) const {
  return ((useCompareMask() & mask) == mask) ? Success() : self() == other.self();
}

}  // namespace overlay
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_OVERLAY_OVLKEYVALUEWMASK_H_ */