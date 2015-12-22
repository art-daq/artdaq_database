#ifndef _ARTDAQ_DATABASE_BASICTYPES_FHICL_H_
#define _ARTDAQ_DATABASE_BASICTYPES_FHICL_H_

#include "artdaq-database/BasicTypes/common.h"

namespace artdaq {
namespace database {
namespace basictypes {

struct FhiclData {
    static constexpr auto type_version() {
        return "V1.0.0";
    }

    std::string fhicl_buffer;
};

} //namespace basictypes
} //namespace database
} //namespace artdaq

#endif /* _ARTDAQ_DATABASE_BASICTYPES_FHICL_H_ */
