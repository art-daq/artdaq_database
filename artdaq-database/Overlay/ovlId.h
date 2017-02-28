#ifndef _ARTDAQ_DATABASE_OVERLAY_OVLOUID_H_
#define _ARTDAQ_DATABASE_OVERLAY_OVLOUID_H_

#include "artdaq-database/Overlay/common.h"
#include "artdaq-database/Overlay/ovlKeyValue.h"

namespace artdaq {
namespace database {
namespace overlay {
namespace jsonliteral = artdaq::database::dataformats::literal;
using namespace artdaq::database::result;

class ovlId final : public ovlKeyValue {
 public:
  ovlId(object_t::key_type const& /*key*/, value_t& /*oid*/);

  bool newId();
  // defaults
  ovlId(ovlId&&) = default;
  ~ovlId() = default;

  // accessors
  std::string& oid();
  std::string& oid(std::string const& /*id*/);

  // ops
  result_t operator==(ovlId const&) const;

 private:
  bool init(value_t& /*parent*/);

 private:
  bool _initOK;
};

using ovlIdUPtr_t = std::unique_ptr<ovlId>;
}  // namespace overlay
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_OVERLAY_OVLOUID_H_ */
