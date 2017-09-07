#ifndef _ARTDAQ_DATABASE_OVERLAY_OVLSTRKEYVALUE_H_
#define _ARTDAQ_DATABASE_OVERLAY_OVLSTRKEYVALUE_H_

#include "artdaq-database/Overlay/common.h"
#include "artdaq-database/Overlay/ovlKeyValue.h"

namespace artdaq {
namespace database {
namespace overlay {
namespace jsonliteral = artdaq::database::dataformats::literal;
using namespace artdaq::database::result;

template <std::uint32_t mask>
class ovlStringKeyValue : public ovlKeyValue {
 public:
  ovlStringKeyValue(object_t::key_type const& /*key*/, value_t& /*value*/);

  // defaults
  ovlStringKeyValue(ovlStringKeyValue&&) = default;
  ~ovlStringKeyValue() = default;

  // ops
  result_t operator==(ovlStringKeyValue const&) const;

 private:
  bool init(value_t& /*parent*/);

 private:
  bool _initOK;
};

template <std::uint32_t mask>
ovlStringKeyValue<mask>::ovlStringKeyValue(object_t::key_type const& key, value_t& value)
    : ovlKeyValue(key, value), _initOK(init(value)) {}

template <std::uint32_t mask>
bool ovlStringKeyValue<mask>::init(value_t& parent) try {
  confirm(type(parent) == type_t::VALUE);

  auto& value = string_value();

  if (value.empty()) value = std::string{jsonliteral::notprovided};

  return true;
} catch (...) {
  confirm(false);
  throw;
}

template <std::uint32_t mask>
result_t ovlStringKeyValue<mask>::operator==(ovlStringKeyValue const& other) const {
  return ((useCompareMask() & mask) == mask) ? Success() : self() == other.self();
}

}  // namespace overlay
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_OVERLAY_OVLSTRKEYVALUE_H_ */