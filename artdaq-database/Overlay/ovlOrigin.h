#ifndef _ARTDAQ_DATABASE_OVERLAY_OVLORIGIN_H_
#define _ARTDAQ_DATABASE_OVERLAY_OVLORIGIN_H_

#include "artdaq-database/Overlay/common.h"
#include "artdaq-database/Overlay/ovlKeyValue.h"
#include "artdaq-database/Overlay/ovlTimeStamp.h"

namespace artdaq {
namespace database {
namespace overlay {
namespace jsonliteral = artdaq::database::dataformats::literal;
using namespace artdaq::database::result;

class ovlOrigin final : public ovlKeyValue {
 public:
  ovlOrigin(object_t::key_type const& /*key*/, value_t& /*origin*/);

  // defaults
  ovlOrigin(ovlOrigin&&) = default;
  ~ovlOrigin() = default;

  // accessors
  std::string& format();
  std::string const& format() const;
  std::string& source();
  std::string const& source() const;

  // overridess
  std::string to_string() const noexcept override;

  // ops
  result_t operator==(ovlOrigin const&) const;

 private:
  ovlTimeStamp map_created(value_t& /*value*/);

  bool init(value_t& /*parent*/);

 private:
  bool _initOK;
  ovlTimeStamp _created;
};

using ovlOriginUPtr_t = std::unique_ptr<ovlOrigin>;

}  // namespace overlay
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_OVERLAY_OVLORIGIN_H_ */
