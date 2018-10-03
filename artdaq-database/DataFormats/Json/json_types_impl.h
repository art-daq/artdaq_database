#ifndef _ARTDAQ_DATABASE_JSONTYPES_IMPL_H_
#define _ARTDAQ_DATABASE_JSONTYPES_IMPL_H_

#include "artdaq-database/DataFormats/Json/json_types.h"


using artdaq::database::json::array_t;
using artdaq::database::json::object_t;
using artdaq::database::json::value_t;

namespace artdaq {
namespace database {
namespace sharedtypes {

template <>
template <typename T>
T& unwrapper<object_t>::value_as(std::string const& name) try {
  confirm(!name.empty());

  return boost::get<T>(any.at(name));
} catch (std::exception& e) {
  throw;
}

template <>
template <typename T>
T& unwrapper<const object_t>::value_as(std::string const& name) try {
  confirm(!name.empty());

  return boost::get<T>(any.at(name));
} catch (std::exception& e) {
  throw;
}

template <>
template <typename T>
T& unwrapper<value_t>::value_as(std::string const& name) try {
  confirm(!name.empty());
  auto& o= boost::get<object_t>(any);
  auto& v = o.at(name);
  return boost::get<T>(v);
} catch (...) {
  throw;
}

template <>
template <typename T>
T& unwrapper<const value_t>::value_as(std::string const& name) try {
  static_assert(std::is_const<T>(),"Template argument T is not a const-qualified type.");
  confirm(!name.empty());
  auto const& o = boost::get<const object_t>(any);
  auto const& v = o.at(name);
  return boost::get<T>(v);
} catch (...) {
  throw;
}

template <>
template <typename T>
T& unwrapper<value_t>::value_as() try {
  return boost::get<T>(any);
} catch (...) {
  throw;
}

template <>
template <typename T>
T& unwrapper<const value_t>::value_as() try {
  static_assert(std::is_const<T>(),"Template argument T is not a const-qualified type.");
  return boost::get<T>(any);
} catch (...) {
  throw;
}


}  // namespace sharedtypes
}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_JSONTYPES_IMPL_H_ */
