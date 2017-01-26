#ifndef _ARTDAQ_DATABASE_OVERLAY_OVLBOOKKEEPING_H_
#define _ARTDAQ_DATABASE_OVERLAY_OVLBOOKKEEPING_H_

#include "artdaq-database/Overlay/common.h"
#include "artdaq-database/Overlay/ovlKeyValue.h"
#include "artdaq-database/Overlay/ovlKeyValueWithDefault.h"
#include "artdaq-database/Overlay/ovlTimeStamp.h"
#include "artdaq-database/Overlay/ovlUpdate.h"

namespace artdaq {
namespace database {
namespace overlay {
namespace jsonliteral = artdaq::database::dataformats::literal;
using namespace artdaq::database::result;

class ovlBookkeeping final : public ovlKeyValue {
  using updates_t = array_t::container_type<ovlUpdate>;

 public:
  ovlBookkeeping(object_t::key_type const& /*key*/, value_t& /*bookkeeping*/);

  // defaults
  ovlBookkeeping(ovlBookkeeping&&) = default;
  ~ovlBookkeeping() = default;

  // accessors
  bool& isReadonly();
  bool const& isReadonly() const;
  bool& isDeleted();
  bool const& isDeleted() const;

  // utils
  bool& markReadonly(bool const& /*state*/);
  bool& markDeleted(bool const& /*state*/);

  template <typename T>
  result_t postUpdate(std::string const& /*name*/, T const& /*what*/);

  // overrides
  std::string to_string() const noexcept override;

  // ops
  result_t operator==(ovlBookkeeping const&) const;

 private:
  updates_t make_updates(value_t& /*value*/);
  ovlTimeStamp map_created(value_t& /*value*/);
  bool init(value_t& /*parent*/);

 private:
  bool _initOK;
  updates_t _updates;
  ovlTimeStamp _created;
};

template <typename T>
result_t ovlBookkeeping::postUpdate(std::string const& name, T const& what) {
  confirm(!name.empty());
  confirm(what);

  using namespace artdaq::database::result;

  if (isReadonly()) return Failure(msg_IsReadonly);

  auto& updates = ovlKeyValue::value_as<array_t>(jsonliteral::updates);

  auto newEntry = object_t{};
  newEntry[jsonliteral::event] = name;
  newEntry[jsonliteral::timestamp] = artdaq::database::timestamp();

  auto const& refvalue = what->value();

  if (type(refvalue) == type_t::OBJECT) {
    newEntry[jsonliteral::value] = refvalue;
  } else {
    auto obj = object_t{};
    obj[jsonliteral::name] = refvalue;
    obj[jsonliteral::assigned] = artdaq::database::timestamp();
    newEntry[jsonliteral::value] = obj;
  }

  value_t tmp = newEntry;

  updates.push_back(tmp);

  // reattach AST
  _updates = make_updates(value());

  return Success(msg_Added);
}

using ovlBookkeepingUPtr_t = std::unique_ptr<ovlBookkeeping>;

}  // namespace overlay
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_OVERLAY_OVLBOOKKEEPING_H_ */
