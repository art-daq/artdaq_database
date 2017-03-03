#include "artdaq-database/DataFormats/common.h"

#include "artdaq-database/DataFormats/Json/json_types.h"

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

namespace json {
constexpr auto noerror = "Success";

struct compare_visitor : public boost::static_visitor<bool> {
  bool operator()(object_t const& first, object_t const& second) const { return (first == second).first; }
  bool operator()(array_t const& first, array_t const& second) const { return (first == second).first; }
  bool operator()(std::string const& first, std::string const& second) const { return (first.compare(second) == 0); }
  bool operator()(double const& first, double const& second) const { return (first == second); }
  bool operator()(int const& first, int const& second) const { return (first == second); }
  bool operator()(bool const& first, bool const& second) const { return (first == second); }
  template <typename A1, typename A2>
  bool operator()(A1 const&, A2 const&) const {
    return false;
  }
};

std::pair<bool, std::string> operator==(value_t const& first, value_t const& second) {
  if (first.which() != second.which())
    return std::make_pair(false, "Values have different types <" + boost::apply_visitor(print_visitor(), first) + "," +
                                     boost::apply_visitor(print_visitor(), second) + ">");

  return boost::apply_visitor(compare_visitor(), first, second)
             ? std::make_pair(true, noerror)
             : std::make_pair(false, "Values are different<" + boost::apply_visitor(print_visitor(), first) + "," +
                                         boost::apply_visitor(print_visitor(), second) + ">");
}

std::pair<bool, std::string> operator==(data_t const& first, data_t const& second) {
  if (first.key.compare(second.key) != 0)
    return std::make_pair(false, "Keys are different <" + first.key + "," + second.key + ">");

  return boost::apply_visitor(compare_visitor(), first.value, second.value)
             ? std::make_pair(true, noerror)
             : std::make_pair(false, "Values are different< " + first.key + ":" +
                                         boost::apply_visitor(print_visitor(), first.value) + "," + second.key + ":" +
                                         boost::apply_visitor(print_visitor(), second.value) + ">");
}

std::pair<bool, std::string> operator==(array_t const& first, array_t const& second) {
  if (first.size() != second.size())
    return std::make_pair(
        false, "Arrays have different sizes <" + print_visitor()(first) + "," + print_visitor()(second) + ">");
  //
  auto first_it = first.begin();
  auto first_end = first.end();

  auto second_it = second.begin();

  for (; first_it != first_end; first_it++, second_it++) {
    if (!boost::apply_visitor(compare_visitor(), *first_it, *second_it))
      return std::make_pair(false,
                            "Arrays are different <" + print_visitor()(first) + "," + print_visitor()(second) + ">");
  }

  return std::make_pair(true, noerror);
}

std::pair<bool, std::string> operator==(object_t const& first, object_t const& second) {
  if (first.size() != second.size())
    return std::make_pair(
        false, "Objects have different sizes <" + print_visitor()(first) + "," + print_visitor()(second) + ">");

  auto first_it = first.begin();
  auto first_end = first.end();

  auto second_it = second.begin();

  for (; first_it != first_end; first_it++, second_it++) {
    auto result = (*first_it == *second_it);
    if (!result.first)
      return std::make_pair(false, "Ojbects are different at key=\"" + first_it->key + "\" " + result.second);
  }

  return std::make_pair(true, noerror);
}
}
}  // namespace database
}  // namespace artdaq
