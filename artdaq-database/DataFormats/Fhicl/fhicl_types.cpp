#include "artdaq-database/DataFormats/common.h"
#include "artdaq-database/DataFormats/Fhicl/fhicl_types.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "fhicl_types.cpp"

namespace artdaq {
namespace database {
namespace sharedtypes {

using artdaq::database::fhicl::value_t;

template <>
 unwrapper<value_t>::unwrapper(value_t& a)
 : any(a) {}

template <>
 unwrapper<const value_t>::unwrapper(const value_t& a)
 : any(a) {}

}  // namespace sharedtypes
}  // namespace database
}  // namespace artdaq
