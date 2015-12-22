#ifndef _ARTDAQ_DATABASE_BASICTYPES_JSON_H_
#define _ARTDAQ_DATABASE_BASICTYPES_JSON_H_

#include "artdaq-database/BasicTypes/common.h"

namespace artdaq{
namespace database{
namespace basictypes{

struct JsonData {  
    explicit JsonData(std::string const&);
    
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

#endif /* _ARTDAQ_DATABASE_BASICTYPES_JSON_H_ */
