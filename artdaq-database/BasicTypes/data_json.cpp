#include "artdaq-database/BasicTypes/data_json.h"
#include <iostream>

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "BTPS:JsonData_C"

namespace artdaq {
namespace database {
namespace basictypes {
  
JsonData::JsonData(std::string const& buffer) : json_buffer{buffer} {}

JsonData::operator std::string const&() const {
  return json_buffer;
}

JsonData::operator std::string&(){
  return json_buffer;
}
  
std::ostream& operator<<(std::ostream& os, JsonData const& data) {
  os << data.json_buffer ;
  return os;
}

bool JsonData::empty() const{
  return json_buffer.empty();
}
std::istream& operator>>(std::istream& is, JsonData& data) {
  data.json_buffer = std::string(std::istreambuf_iterator<char>(is), {});
  return is;
}

}  // namespace basictypes
}  // namespace database
}  // namespace artdaq
