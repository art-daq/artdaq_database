#ifndef _ARTDAQ_DATABASE_BASICTYPES_FHICL_H_
#define _ARTDAQ_DATABASE_BASICTYPES_FHICL_H_

#include "artdaq-database/BasicTypes/common.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "data_fhicl.h"

namespace artdaq {
namespace database {
namespace basictypes {

struct JsonData;

struct FhiclData final {
  FhiclData(std::string);

  FhiclData() = default;

  FhiclData(JsonData const&);
  operator JsonData() const;
  operator std::string const&() const;

  static constexpr auto type_version() { return "V100"; }

  std::string fhicl_buffer = "";
  std::string fhicl_file_name = "notprovided";
};

std::istream& operator>>(std::istream&, artdaq::database::basictypes::FhiclData&);
std::ostream& operator<<(std::ostream&, artdaq::database::basictypes::FhiclData const&);

}  // namespace basictypes
}  // namespace database
}  // namespace artdaq

namespace {
template<>
inline TraceStreamer& TraceStreamer::operator<<(const artdaq::database::basictypes::FhiclData& r)
{
  std::ostringstream s; s << r; msg_append(s.str().c_str());
  return *this;
}
}
#endif /* _ARTDAQ_DATABASE_BASICTYPES_FHICL_H_ */
