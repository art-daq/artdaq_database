#ifndef _ARTDAQ_DATABASE_FHICLJSON_CONVERTFHICL2JSONGUI_H_
#define _ARTDAQ_DATABASE_FHICLJSON_CONVERTFHICL2JSONGUI_H_

#include "artdaq-database/FhiclJson/common.h"

#include "artdaq-database/FhiclJson/fhicl_types.h"
#include "artdaq-database/FhiclJson/json_types.h"
#include "fhiclcpp/extended_value.h"

namespace artdaq {
namespace database {
namespace fhicljson {

namespace fcl = artdaq::database::fhicl;
namespace jsn = artdaq::database::json;

using comment_t = std::string;
using linenum_t = int;
using comments_t = std::map<linenum_t, comment_t>;

struct fcl2jsongui final {
  using fcl_value = ::fhicl::extended_value;
  using fhicl_key_value_pair_t = std::pair<std::string, fcl_value> const;

  explicit fcl2jsongui(fhicl_key_value_pair_t const&, fhicl_key_value_pair_t const&, comments_t const&);

  operator jsn::object_t();

  fhicl_key_value_pair_t const& self;
  fhicl_key_value_pair_t const& parent;
  comments_t const& comments;
};

struct json2fclgui final {
  explicit json2fclgui(jsn::value_t const&, jsn::value_t const&);

  operator fcl::value_t();
  operator fcl::atom_t();

  jsn::value_t const& self;
  jsn::value_t const& parent;
};

}  // namespace fhicljson
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_FHICLJSON_CONVERTFHICL2JSONGUI_H_ */
