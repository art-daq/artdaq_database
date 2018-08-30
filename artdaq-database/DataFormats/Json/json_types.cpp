#include "artdaq-database/DataFormats/common.h"

#include "artdaq-database/DataFormats/Json/json_types.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "json_types.cpp"

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
/*
template <>
template <>
jsn::object_t const& unwrapper<const jsn::object_t>::value_as<const jsn::object_t>(std::string const& name)  try {
  confirm(!name.empty());
  return boost::get<jsn::object_t>(any.at(name));
} catch (...) {
  throw;
}

template <>
template <>
std::string const& unwrapper<const jsn::object_t>::value_as<const std::string>(std::string const& name)  try {
  confirm(!name.empty());
  return boost::get<std::string>(any.at(name));
} catch (...) {
  throw;
}

template <>
template <>
jsn::array_t const& unwrapper<const jsn::object_t>::value_as<const jsn::array_t>(std::string const& name)  try {
  confirm(!name.empty());
  return boost::get<jsn::array_t>(any.at(name));
} catch (...) {
  throw;
}
*/
}  // namespace sharedtypes

namespace json {
constexpr auto noerror = "Success";

struct compare_visitor : public boost::static_visitor<bool> {
  bool operator()(object_t const& first, object_t const& second) const { return (first == second).first; }
  bool operator()(array_t const& first, array_t const& second) const { return (first == second).first; }
  bool operator()(std::string const& first, std::string const& second) const { return (first == second); }
  bool operator()(decimal const& first, decimal const& second) const { return (first == second); }
  bool operator()(integer const& first, integer const& second) const { return (first == second); }
  bool operator()(bool const& first, bool const& second) const { return (first == second); }
  template <typename A1, typename A2>
  bool operator()(A1 const& /*unused*/, A2 const& /*unused*/) const {
    return false;
  }
};

std::pair<bool, std::string> operator==(value_t const& first, value_t const& second) {
  if (first.which() != second.which()) {
    return std::make_pair(false, "Values have different types <" + boost::apply_visitor(print_visitor(), first) + "," +
                                     boost::apply_visitor(print_visitor(), second) + ">");
  }

  return boost::apply_visitor(compare_visitor(), first, second)
             ? std::make_pair(true, noerror)
             : std::make_pair(false, "Values are different<" + boost::apply_visitor(print_visitor(), first) + "," +
                                         boost::apply_visitor(print_visitor(), second) + ">");
}

std::pair<bool, std::string> operator==(data_t const& first, data_t const& second) {
  if (first.key != second.key) {
    return std::make_pair(false, "Keys are different <" + first.key + "," + second.key + ">");
  }

  return boost::apply_visitor(compare_visitor(), first.value, second.value)
             ? std::make_pair(true, noerror)
             : std::make_pair(false, "Values are different< " + first.key + ":" + boost::apply_visitor(print_visitor(), first.value) + "," +
                                         second.key + ":" + boost::apply_visitor(print_visitor(), second.value) + ">");
}

std::pair<bool, std::string> operator==(array_t const& first, array_t const& second) {
  if (first.size() != second.size()) {
    return std::make_pair(false, "Arrays have different sizes <" + print_visitor()(first) + "," + print_visitor()(second) + ">");
  }
  //
  auto first_it = first.begin();
  auto first_end = first.end();

  auto second_it = second.begin();

  for (; first_it != first_end; first_it++, second_it++) {
    if (!boost::apply_visitor(compare_visitor(), *first_it, *second_it)) {
      return std::make_pair(false, "Arrays are different <" + print_visitor()(first) + "," + print_visitor()(second) + ">");
    }
  }

  return std::make_pair(true, noerror);
}

std::pair<bool, std::string> operator==(object_t const& first, object_t const& second) {
  if (first.size() != second.size()) {
    return std::make_pair(false, "Objects have different sizes <" + print_visitor()(first) + "," + print_visitor()(second) + ">");
  }

  auto first_it = first.begin();
  auto first_end = first.end();

  auto second_it = second.begin();

  for (; first_it != first_end; first_it++, second_it++) {
    auto result = (*first_it == *second_it);
    if (!result.first) {
      return std::make_pair(false, "Ojbects are different at key=\"" + first_it->key + "\" " + result.second);
    }
  }

  return std::make_pair(true, noerror);
}

std::string to_string(type_t t) {
  switch (t) {
    case type_t::NOTSET:
      return "NOTSET";
    case type_t::VALUE:
      return "VALUE";
    case type_t::DATA:
      return "DATA";
    case type_t::OBJECT:
      return "OBJECT";
    case type_t::ARRAY:
      return "ARRAY";
  }
  return "NOTSET";
}

}  // namespace json
}  // namespace database
}  // namespace artdaq
