#ifndef _ARTDAQ_DATABASE_OVERLAY_OVLORIGIN_H_
#define _ARTDAQ_DATABASE_OVERLAY_OVLORIGIN_H_

#include "artdaq-database/Overlay/common.h"
#include "artdaq-database/Overlay/ovlFixedList.h"
#include "artdaq-database/Overlay/ovlKeyValue.h"
#include "artdaq-database/Overlay/ovlStringKeyValue.h"
#include "artdaq-database/Overlay/ovlTimeStamp.h"

namespace artdaq {
namespace database {
namespace overlay {
namespace jsonliteral = artdaq::database::dataformats::literal;
using namespace artdaq::database::result;

using ovlRawData = ovlStringKeyValue<DOCUMENT_COMPARE_MUTE_RAWDATA>;
using ovlRawDataUPtr_t = std::unique_ptr<ovlRawData>;

using ovlRawDataList = ovlFixedList<ovlRawData, DOCUMENT_COMPARE_MUTE_SEARCH>;
using ovlRawDataListUPtr_t = std::unique_ptr<ovlRawDataList>;

class ovlOrigin final : public ovlKeyValue {
 public:
  ovlOrigin(object_t::key_type const& /*key*/, value_t& /*origin*/);

  // defaults
  ovlOrigin(ovlOrigin&&) = default;
  ~ovlOrigin() = default;

  // accessors
  std::string& format();
  std::string const& format() const;

  std::string& name();
  std::string const& name() const;

  std::string& source();
  std::string const& source() const;

  // overridess
  std::string to_string() const override;

  // ops
  result_t operator==(ovlOrigin const&) const;

 private:
  ovlTimeStamp map_created(value_t& /*value*/);

  bool init(value_t& /*parent*/);

 private:
  bool _initOK;
  ovlTimeStamp _created;
  ovlRawDataListUPtr_t _rawdatalist;
};

using ovlOriginUPtr_t = std::unique_ptr<ovlOrigin>;

}  // namespace overlay
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_OVERLAY_OVLORIGIN_H_ */
