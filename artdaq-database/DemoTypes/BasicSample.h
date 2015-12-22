#ifndef _DEMOCONFIGURATIONDB_BASICSAMPLE_H_
#define _DEMOCONFIGURATIONDB_BASICSAMPLE_H_

#include "artdaq-database/DemoTypes/common.h"

namespace artdaq {
namespace database {
namespace demo {

using namespace artdaq::database::demo;

struct BasicSampleB final {
    int         f1;
    double      f2;
};

struct BasicSample final {
    static constexpr auto type_version() {
        return "V1.0.0";
    }
    int         value1;
    double      value2;
    std::string value3;

    //std::vector<BasicSampleB> value4= { {34,12.2L}, {34,14.5L}, {4,34.6L} };
    std::vector<int> value5;
};


} //namespace demo
} //namespace database
} //namespace artdaq

#endif /* _DEMOCONFIGURATIONDB_BASICSAMPLE_H_ */
