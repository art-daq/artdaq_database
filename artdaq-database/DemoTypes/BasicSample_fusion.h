#ifndef _DEMOCONFIGURATIONDB_BASICSAMPLE_FUSION_H_
#define _DEMOCONFIGURATIONDB_BASICSAMPLE_FUSION_H_

#include "artdaq-database/DemoTypes/BasicSample.h"

#include <boost/fusion/adapted/struct/adapt_assoc_struct.hpp>
#include <boost/fusion/include/adapt_assoc_struct.hpp>

using namespace artdaq::database::demo;

namespace cfg = artdaq::database::demo;

namespace BasicSampleBKeys
{
  namespace keys {
      struct f1;
      struct f2;
  }
}

BOOST_FUSION_ADAPT_ASSOC_STRUCT(
    cfg::BasicSampleB,
    (int,            f1,	BasicSampleBKeys::keys::f1)
    (double,         f2,	BasicSampleBKeys::keys::f2) 
)

//used by boost_unit_test
inline bool operator==(const BasicSampleB & r, const BasicSampleB & l)
{
  return ( r.f1 == l.f1 &&
           r.f2 == l.f2
  );
}

namespace BasicSampleKeys
{
  namespace keys {
      struct value1;
      struct value2;
      struct value3;
      struct value4;
      struct value5;
  }
}

BOOST_FUSION_ADAPT_ASSOC_STRUCT(
    cfg::BasicSample,
    (int,            value1,	BasicSampleKeys::keys::value1)
    (double,         value2,	BasicSampleKeys::keys::value2) 
    (std::string,    value3,	BasicSampleKeys::keys::value3)
    //(std::vector<BasicSampleB>,   value4,	BasicSampleKeys::keys::value4) 
    (std::vector<int>,   value5,	BasicSampleKeys::keys::value5) 
)

//used by boost_unit_test
inline bool operator==(const BasicSample & r, const BasicSample & l)
{
  return ( r.value1 == l.value1 &&
           r.value2 == l.value2 &&
           r.value3 == l.value3 &&
          // r.value4 == l.value4 &&
           r.value5 == l.value5
  );
}


#endif /* _DEMOCONFIGURATIONDB_BASICSAMPLE_FUSION_H_ */
