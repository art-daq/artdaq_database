#ifndef _ARTDAQ_DATABASE_OVERLAY_OVLCHNAGELOG_H_
#define _ARTDAQ_DATABASE_OVERLAY_OVLCHNAGELOG_H_

#include "artdaq-database/Overlay/common.h"
#include "artdaq-database/Overlay/ovlKeyValue.h"
#include "artdaq-database/Overlay/ovlStringKeyValue.h"

namespace artdaq {
namespace database {
namespace overlay {
namespace jsonliteral = artdaq::database::dataformats::literal;
using namespace artdaq::database::result;

class ovlChangeLog final : public ovlStringKeyValue<DOCUMENT_COMPARE_MUTE_CHANGELOG> {
 public:
  ovlChangeLog(object_t::key_type const& /*key*/, value_t& /*changelog*/);

  // defaults
  ovlChangeLog(ovlChangeLog&&) = default;
  ~ovlChangeLog() = default;

  // accessors
  std::string& buffer();
  std::string const& buffer() const;
  std::string& buffer(std::string const& /*changelog*/);

  // utils
  std::string& append(std::string const& /*changelog*/);

  result_t operator==(ovlChangeLog const& /*other*/) const;
};

using ovlChangeLogUPtr_t = std::unique_ptr<ovlChangeLog>;

}  // namespace overlay
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_OVERLAY_OVLCHNAGELOG_H_ */
