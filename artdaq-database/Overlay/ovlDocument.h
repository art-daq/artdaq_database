#ifndef _ARTDAQ_DATABASE_OVERLAY_OVLDOCUMENT_H_
#define _ARTDAQ_DATABASE_OVERLAY_OVLDOCUMENT_H_

#include "artdaq-database/Overlay/common.h"
#include "artdaq-database/Overlay/ovlFixedList.h"
#include "artdaq-database/Overlay/ovlKeyValue.h"
#include "artdaq-database/Overlay/ovlKeyValueWithMask.h"

namespace artdaq {
namespace database {
namespace overlay {
namespace jsonliteral = artdaq::database::dataformats::literal;
using namespace artdaq::database::result;

using ovlData = ovlKeyValueWithMask<DOCUMENT_COMPARE_MUTE_DATA>;
using ovlDataUPtr_t = std::unique_ptr<ovlData>;

using ovlMetadata = ovlKeyValueWithMask<DOCUMENT_COMPARE_MUTE_METADATA>;
using ovlMetadataUPtr_t = std::unique_ptr<ovlMetadata>;

using ovlSearch = ovlKeyValue;
using ovlSearchUPtr_t = std::unique_ptr<ovlSearch>;

using ovlSearches = ovlFixedList<ovlSearch, DOCUMENT_COMPARE_MUTE_SEARCH>;
using ovlSearchesUPtr_t = std::unique_ptr<ovlSearches>;

class ovlDocument final : public ovlKeyValue {
 public:
  ovlDocument(object_t::key_type const& /*key*/, value_t& /*document*/);

  // defaults
  ovlDocument(ovlDocument&&) = default;
  ~ovlDocument() = default;

  // accessors
  ovlData& data();
  ovlData const& data() const;

  ovlMetadata& metadata();
  ovlMetadata const& metadata() const;

  // utils
  void swap(ovlDataUPtr_t& /*data*/);
  void swap(ovlMetadataUPtr_t& /*metadata*/);

  void make_empty();

  // overridess
  std::string to_string() const noexcept override;

  // ops
  result_t operator==(ovlDocument const&) const;

 private:
  ovlDataUPtr_t _data;
  ovlMetadataUPtr_t _metadata;
  ovlSearchesUPtr_t _search;
};

using ovlDocumentUPtr_t = std::unique_ptr<ovlDocument>;

}  // namespace overlay
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_OVERLAY_OVLDOCUMENT_H_ */