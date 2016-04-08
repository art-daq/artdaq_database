#include "artdaq-database/FhiclJson/common.h"

#include "artdaq-database/FhiclJson/fhicl_types.h"
#include "artdaq-database/FhiclJson/json_types.h"
#include "artdaq-database/FhiclJson/shared_types.h"

using artdaq::database::sharedtypes::unwrap;
using artdaq::database::sharedtypes::unwrapper;
namespace artdaq {
namespace database {
namespace sharedtypes {

template <>
template <>
jsn::object_t& unwrapper<jsn::object_t&>::value_as() {
  return boost::get<jsn::object_t&>(any);
}
template <>
template <>
jsn::array_t& unwrapper<jsn::array_t&>::value_as() {
  return boost::get<jsn::array_t&>(any);
}
}  // namespace sharedtypes
}  // namespace database
}  // namespace artdaq

/*
template<>
template<typename T>
T&  unwrapper<jsn::value_t>::value_as() {
     return boost::get<T>( any );
}

template<>
template<typename T>
T const&  unwrapper<const jsn::value_t>::value_as() {
     using V = typename std::remove_const<T>::type;

     return boost::get<V>( any );
}

template<>
template<typename T>
T&  unwrapper<fcl::value_t>::value_as() {
     return boost::get<T>( any.value );
}


template<>
template<typename T>
T const&  unwrapper<const jsn::object_t>::value_as( std::string const& name ) {
     assert( !name.empty() );

     using V = typename std::remove_const<T>::type;

     return boost::get<V>( any.at( name ) );
}

template<>
template<typename T>
T&  unwrapper<jsn::object_t>::value_as( std::string const& name ) {
     assert( !name.empty() );

     return boost::get<T>( any.at( name ) );
}
*/