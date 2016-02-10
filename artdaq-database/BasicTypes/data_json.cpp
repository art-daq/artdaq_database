#include "artdaq-database/BasicTypes/data_json.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "BTYPES:JsonData_C"

using artdaq::database::basictypes::JsonData;

JsonData::JsonData(std::string const& buffer)
    : json_buffer {buffer} {

}


std::ostream& operator<<(std::ostream& os, JsonData const& data)
{
    os << data.json_buffer;
    return os;
}

std::istream& operator>>(std::istream& is, JsonData& data)
{
    data.json_buffer = std::string(std::istreambuf_iterator<char>(is), {});
    return is;
}
