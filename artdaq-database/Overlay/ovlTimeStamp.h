#ifndef _ARTDAQ_DATABASE_OVERLAY_OVLTIMESTAMP_H_
#define _ARTDAQ_DATABASE_OVERLAY_OVLTIMESTAMP_H_

#include "artdaq-database/Overlay/common.h"
#include "artdaq-database/Overlay/ovlKeyValue.h"

namespace artdaq {
namespace database {
namespace overlay {
namespace jsonliteral = artdaq::database::dataformats::literal;

class ovlTimeStamp final : public ovlKeyValue {
 public:
  ovlTimeStamp(object_t::key_type const& /*key*/, value_t& /*ts*/);

  // defaults
  ovlTimeStamp(ovlTimeStamp&&) = default;
  ~ovlTimeStamp() = default;

  // accessors
  std::string& timestamp();
  std::string const& timestamp() const;
  std::string& timestamp(std::string const& /*ts*/);

  // overridess
  std::string to_string() const override;

  // ops
  result_t operator==(ovlTimeStamp const&) const;
};
using ovlTimeStampUPtr_t = std::unique_ptr<ovlTimeStamp>;

}  // namespace overlay
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_OVERLAY_OVLTIMESTAMP_H_ */