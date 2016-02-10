#include "artdaq-database/FhiclJson/common.h"

#include "artdaq-database/FhiclJson/fhicl_types.h"
#include "artdaq-database/FhiclJson/json_types.h"
#include "artdaq-database/FhiclJson/shared_types.h"

using artdaq::database::sharedtypes::unwrap;
using artdaq::database::sharedtypes::unwrapper;

template<>
template<typename T>
T&  unwrapper<jsn::value_t>::value_as()
{
    return boost::get<T>(any);
}

template<>
template<typename T>
T const&  unwrapper<const jsn::value_t>::value_as()
{
    using V = typename std::remove_const<T>::type;

    return boost::get<V>(any);
}

template<>
template<typename T>
T&  unwrapper<fcl::value_t>::value_as()
{
    return boost::get<T>(any.value);
}
