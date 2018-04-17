#ifndef _ARTDAQ_DATABASE_BASICTYPES_XML_H_
#define _ARTDAQ_DATABASE_BASICTYPES_XML_H_

#include "artdaq-database/BasicTypes/common.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "BTPS:XmlData_H"

namespace artdaq {
namespace database {
namespace basictypes {

struct JsonData;

struct XmlData final {
  XmlData(std::string);

  XmlData() = default;

  XmlData(JsonData const&);
  operator JsonData() const;
  operator std::string const&() const;

  static constexpr auto type_version() { return "V100"; }

  std::string xml_buffer;
};

std::istream& operator>>(std::istream&, artdaq::database::basictypes::XmlData&);
std::ostream& operator<<(std::ostream&, artdaq::database::basictypes::XmlData const&);

}  // namespace basictypes
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_BASICTYPES_FHICL_H_ */
