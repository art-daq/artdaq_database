#ifndef _ARTDAQ_DATABASE_OVERLAY_OVLUPDATE_H_
#define _ARTDAQ_DATABASE_OVERLAY_OVLUPDATE_H_

#include "artdaq-database/Overlay/common.h"
#include "artdaq-database/Overlay/ovlKeyValue.h"
#include "artdaq-database/Overlay/ovlKeyValueTimeStamp.h"
#include "artdaq-database/Overlay/ovlTimeStamp.h"

namespace artdaq {
namespace database {
namespace overlay {
namespace jsonliteral = artdaq::database::dataformats::literal;
using namespace artdaq::database::result;

using ovlUpdateEntry = ovlKeyValueTimeStamp<DOCUMENT_COMPARE_MUTE_UPDATE_VALUES>;
using ovlUpdateEntryUPtr_t = std::unique_ptr<ovlUpdateEntry>;

class ovlUpdate final : public ovlKeyValue {
 public:
  ovlUpdate(object_t::key_type const& /*key*/, value_t& /*update*/);

  // defaults
  ovlUpdate(ovlUpdate&&) = default;
  ~ovlUpdate() = default;

  // accessors
  std::string& name();
  std::string const& name() const;
  std::string& timestamp();
  std::string const& timestamp() const;

  ovlUpdateEntry& what();
  ovlUpdateEntry const& what() const;

  // overrides
  std::string to_string() const override;

  // ops
  result_t operator==(ovlUpdate const&) const;

 private:
  ovlUpdateEntry map_what(value_t& /*value*/);
  ovlTimeStamp map_timestamp(value_t& /*value*/);

 private:
  ovlTimeStamp _timestamp;
  ovlUpdateEntry _what;
};

using ovlUpdateUPtr_t = std::unique_ptr<ovlUpdate>;

}  // namespace overlay
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_OVERLAY_OVLUPDATE_H_ */
