#ifndef _ARTDAQ_DATABASE_OVERLAY_OVLCOMMENT_H_
#define _ARTDAQ_DATABASE_OVERLAY_OVLCOMMENT_H_

#include "artdaq-database/Overlay/common.h"
#include "artdaq-database/Overlay/ovlKeyValue.h"

namespace artdaq {
namespace database {
namespace overlay {
namespace jsonliteral = artdaq::database::dataformats::literal;
using namespace artdaq::database::result;

class ovlComment final : public ovlKeyValue {
 public:
  ovlComment(object_t::key_type const& /*key*/, value_t& /*comment*/);

  // defaults
  ovlComment(ovlComment&&) = default;
  ~ovlComment() = default;

  // accessors
  int& linenum();
  std::string& text();
  int const& linenum() const;
  std::string const& text() const;

  // overridess
  std::string to_string() const override;

  // ops
  result_t operator==(ovlComment const&) const;
};

using ovlCommentUPtr_t = std::unique_ptr<ovlComment>;

}  // namespace overlay
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_OVERLAY_OVLCOMMENT_H_ */
