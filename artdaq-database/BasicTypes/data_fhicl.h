#ifndef _ARTDAQ_DATABASE_BASICTYPES_FHICL_H_
#define _ARTDAQ_DATABASE_BASICTYPES_FHICL_H_

#include "artdaq-database/BasicTypes/common.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "FhiclData_H"

namespace artdaq{
namespace database{
namespace basictypes{

struct JsonData;

struct FhiclData {
    FhiclData(std::string const&);

    FhiclData(JsonData const&);
    operator JsonData() const;

    static constexpr auto type_version() {
        return "V100";
    }

    std::string fhicl_buffer;
};

} //namespace basictypes
} //namespace database
} //namespace artdaq

#endif /* _ARTDAQ_DATABASE_BASICTYPES_FHICL_H_ */
