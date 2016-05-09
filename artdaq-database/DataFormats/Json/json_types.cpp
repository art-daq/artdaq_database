#include "artdaq-database/DataFormats/common.h"

#include "artdaq-database/DataFormats/Json/json_types.h"
#include "artdaq-database/DataFormats/common/shared_types.h"

using artdaq::database::sharedtypes::unwrap;
using artdaq::database::sharedtypes::unwrapper;
namespace artdaq {
namespace database {
namespace sharedtypes {

template <>
template <>
jsn::object_t& unwrapper<jsn::object_t&>::value_as() {
  return boost::get<jsn::object_t&>(any);
}
template <>
template <>
jsn::array_t& unwrapper<jsn::array_t&>::value_as() {
  return boost::get<jsn::array_t&>(any);
}
}  // namespace sharedtypes
}  // namespace database
}  // namespace artdaq
