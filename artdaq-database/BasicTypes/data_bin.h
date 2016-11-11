#ifndef _ARTDAQ_DATABASE_BASICTYPES_BIN_H_
#define _ARTDAQ_DATABASE_BASICTYPES_BIN_H_

#include "artdaq-database/BasicTypes/common.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "BTYPES:BinData_H"

namespace artdaq {
namespace database {
namespace basictypes {

struct JsonData;

struct BinData {
  BinData(std::vector<unsigned char> const&);

  BinData() = default;

  BinData(JsonData const&);
  operator JsonData() const;

  static constexpr auto type_version() { return "V100"; }
  
  std::vector<unsigned char> bin_buffer;
};

std::istream& operator>>(std::istream&, artdaq::database::basictypes::BinData&);
std::ostream& operator<<(std::ostream&, artdaq::database::basictypes::BinData const&);


}  // namespace basictypes
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_BASICTYPES_BIN_H_ */
