#ifndef _ARTDAQ_DATABASE_BASICTYPES_JSON_H_
#define _ARTDAQ_DATABASE_BASICTYPES_JSON_H_

#include "artdaq-database/BasicTypes/common.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "data_json.h"

namespace artdaq {
namespace database {
namespace basictypes {

struct JsonData final {
  JsonData(std::string);

  template <typename TYPE>
  bool convert_to(TYPE&) const;

  template <typename TYPE>
  bool convert_from(TYPE const&);

  operator std::string const&() const;
  operator std::string&();

  bool empty() const;

  static constexpr auto type_version() { return "V1.0.0"; }

  std::string json_buffer;
};

std::ostream& operator<<(std::ostream&, artdaq::database::basictypes::JsonData const&);
std::istream& operator>>(std::istream&, artdaq::database::basictypes::JsonData&);

}  // namespace basictypes
}  // namespace database
}  // namespace artdaq

namespace {
template <>
inline TraceStreamer& TraceStreamer::operator<<(const artdaq::database::basictypes::JsonData& r) {
  std::ostringstream s;
  s << r;
  msg_append(s.str().c_str());
  return *this;
}
}  // namespace
#endif /* _ARTDAQ_DATABASE_BASICTYPES_JSON_H_ */
