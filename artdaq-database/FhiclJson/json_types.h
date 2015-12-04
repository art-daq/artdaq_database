#ifndef _ARTDAQ_DATABASE_JSONTYPES_H_
#define _ARTDAQ_DATABASE_JSONTYPES_H_

#include "artdaq-database/FhiclJson/common.h"
#include "artdaq-database/FhiclJson/shared_types.h"

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

namespace artdaq{
namespace database{
namespace json{

using namespace artdaq::database;

struct object_t;
struct array_t;

using variant_value_t = sharedtypes::variant_value_of<object_t, array_t>;

using key_t = sharedtypes::basic_key_t;
using value_t = variant_value_t;
using data_t = sharedtypes::kv_pair_of<key_t, value_t>;

struct object_t : sharedtypes::table_of<data_t> {};
struct array_t : sharedtypes::vector_of<value_t> {};

} //namespace json
} //namespace database
} //namespace artdaq

namespace jsn = artdaq::database::json;

BOOST_FUSION_ADAPT_STRUCT(
    jsn::data_t,
    (jsn::key_t, key)
    (jsn::value_t, value)
)

#endif /* _ARTDAQ_DATABASE_JSONTYPES_H_ */

