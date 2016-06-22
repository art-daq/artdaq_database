#ifndef _ARTDAQ_DATABASE_JSONTYPES_H_
#define _ARTDAQ_DATABASE_JSONTYPES_H_

#include "artdaq-database/DataFormats/common.h"
#include "artdaq-database/DataFormats/common/shared_types.h"

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

namespace artdaq {
namespace database {
namespace json {

using namespace artdaq::database;

struct object_t;
struct array_t;

using variant_value_t = sharedtypes::variant_value_of<object_t, array_t>;

using key_t = sharedtypes::basic_key_t;
using value_t = variant_value_t;
using data_t = sharedtypes::kv_pair_of<key_t, value_t>;

struct object_t : sharedtypes::table_of<data_t> {};
struct array_t : sharedtypes::vector_of<value_t> {};

enum struct type_t { NOTSET = 0, VALUE, DATA, OBJECT, ARRAY };

struct print_visitor : public boost::static_visitor<std::string> {
  std::string operator()(object_t const&) const { return "object(...)"; }
  std::string operator()(array_t const&) const { return "array(...)"; }
  std::string operator()(std::string const& val) const { std::stringstream ss; ss << "std::string(" << val << ")";  return ss.str();}
  std::string operator()(double const& val) const { return "double(" + std::to_string(val) + ")"; }
  std::string operator()(int const& val) const { return "int(" + std::to_string(val) + ")"; }
  std::string operator()(bool const& val) const { return val ? "bool(true)" : "bool(false)"; }
};

struct tostring_visitor : public boost::static_visitor<std::string> {
  std::string operator()(object_t const&) const { return "object(...)"; }
  std::string operator()(array_t const&) const { return "array(...)"; }
  std::string operator()(std::string const& val) const { return val; }
  std::string operator()(double const& val) const { return std::to_string(val);}
  std::string operator()(int const& val) const { return std::to_string(val);}
  std::string operator()(bool const& val) const { return val ? "true" : "false"; }
};

std::pair<bool, std::string> operator==(value_t const&, value_t const&);
std::pair<bool, std::string> operator==(data_t const&, data_t const&);
std::pair<bool, std::string> operator==(array_t const&, array_t const&);
std::pair<bool, std::string> operator==(object_t const&, object_t const&);

}  // namespace json
}  // namespace database
}  // namespace artdaq

namespace jsn = artdaq::database::json;

BOOST_FUSION_ADAPT_STRUCT(jsn::data_t, (jsn::key_t, key)(jsn::value_t, value))

#endif /* _ARTDAQ_DATABASE_JSONTYPES_H_ */
