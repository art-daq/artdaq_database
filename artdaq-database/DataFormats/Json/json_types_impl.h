#ifndef _ARTDAQ_DATABASE_JSONTYPES_IMPL_H_
#define _ARTDAQ_DATABASE_JSONTYPES_IMPL_H_

#include "artdaq-database/DataFormats/Json/json_types.h"

namespace dbt = artdaq::database::sharedtypes;

using artdaq::database::json::array_t;
using artdaq::database::json::value_t;
using artdaq::database::json::object_t;

template <>
template <typename T>
T const& dbt::unwrapper<const object_t>::value_as(std::string const& name) try {
  confirm(!name.empty());

  using V = typename std::remove_const<T>::type;

  return boost::get<V>(any.at(name));
} catch (...) {
  throw;
}

template <>
template <typename T>
T& dbt::unwrapper<object_t>::value_as(std::string const& name) try {
  confirm(!name.empty());

  return boost::get<T>(any.at(name));
} catch (std::exception &e) {
  throw;
}

template <>
template <typename T>
T& dbt::unwrapper<value_t>::value_as(std::string const& name) try {
  confirm(!name.empty());
  return boost::get<T>(boost::get<object_t>(any).at(name));
} catch (...) {
  throw;
}

template <>
template <typename T>
T const& dbt::unwrapper<const value_t>::value_as(std::string const& name) try {
  confirm(!name.empty());

  using V = typename std::remove_const<T>::type;

  return boost::get<V>(boost::get<object_t>(any).at(name));
} catch (...) {
  throw;
}

template <>
template <typename T>
T& dbt::unwrapper<value_t>::value_as() try {
  return boost::get<T>(any);
} catch (...) {
  throw;
}

#endif /* _ARTDAQ_DATABASE_JSONTYPES_IMPL_H_ */