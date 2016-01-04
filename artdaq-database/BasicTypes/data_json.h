#ifndef _ARTDAQ_DATABASE_BASICTYPES_JSON_H_
#define _ARTDAQ_DATABASE_BASICTYPES_JSON_H_

#include "artdaq-database/BasicTypes/common.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "JsonData_H"


namespace artdaq{
namespace database{
namespace basictypes{

struct JsonData {  
    JsonData(std::string const&);
    
    template<typename TYPE>
    bool convert_to(TYPE&) const;

    template<typename TYPE>
    bool convert_from(TYPE const&);

    static constexpr auto type_version() {
        return "V1.0.0";
    }

    std::string json_buffer;
};


} //namespace basictypes
} //namespace database
} //namespace artdaq

std::ostream& operator<<(std::ostream&, artdaq::database::basictypes::JsonData const&);
std::istream& operator>>(std::istream&, artdaq::database::basictypes::JsonData&);

#endif /* _ARTDAQ_DATABASE_BASICTYPES_JSON_H_ */
