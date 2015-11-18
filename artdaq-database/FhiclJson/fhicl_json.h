#ifndef _ARTDAQ_DATABASE_FHICLJSON_H_
#define _ARTDAQ_DATABASE_FHICLJSON_H_


#include "artdaq-database/FhiclJson/common.h"

#include <boost/variant/recursive_variant.hpp>
#include <boost/fusion/include/adapt_assoc_struct.hpp>
#include <boost/optional.hpp>

namespace artdaq{
namespace database{
namespace fhicljson{

namespace keys{
  struct key;
  struct value;
}

struct table_t;
struct sequence_t;

using any_key_t   =  std::string;
using any_value_t =  boost::variant <
			boost::recursive_wrapper<table_t>,
			boost::recursive_wrapper<sequence_t>,
			std::string,
			double,
			int,
			bool
			>; 

struct table_t    :std::map< any_key_t, any_value_t> {};
struct sequence_t :std::vector<any_value_t> {};
using  atom_t  =   std::pair<any_key_t, any_value_t>;


} //namespace fhicljson
} //namespace database
} //namespace artdaq

using namespace  artdaq::database::fhicljson;


BOOST_FUSION_ADAPT_ASSOC_STRUCT(
    atom_t,
    (any_key_t,   first,  keys::key)
    (any_value_t, second, keys::value)
)

namespace literal{
constexpr auto data_node = "data";
constexpr auto comments_node = "comments";
constexpr auto origin_node = "origin";
constexpr auto timestamp = "timestamp";
constexpr auto source = "source";
constexpr auto file = "file";
constexpr auto value = "value";
constexpr auto type = "type";
constexpr auto whitespace = " ";
constexpr auto empty = "";
constexpr auto comment = "comment";
constexpr auto annotaion = "annotaion";

}

#endif /* _ARTDAQ_DATABASE_FHICLJSON_H_ */

