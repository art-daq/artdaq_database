#ifndef _ARTDAQ_DATABASE_DATAFORMATS_FHICL_CONVERTFHICL2JSONDB_H_
#define _ARTDAQ_DATABASE_DATAFORMATS_FHICL_CONVERTFHICL2JSONDB_H_

#include "artdaq-database/DataFormats/Fhicl/fhiclcpplib_includes.h"
#include "artdaq-database/DataFormats/common.h"
#include "artdaq-database/DataFormats/shared_literals.h"

#include "artdaq-database/DataFormats/Fhicl/fhicl_reader.h"
#include "artdaq-database/DataFormats/Fhicl/fhicl_types.h"
#include "artdaq-database/DataFormats/Json/json_types.h"

namespace artdaq {
namespace database {
namespace fhicljson {

namespace fcl = artdaq::database::fhicl;
namespace jsn = artdaq::database::json;

namespace literal = artdaq::database::dataformats::literal;

using artdaq::database::fhicl::comment_t;
using artdaq::database::fhicl::comments_t;
using artdaq::database::fhicl::linenum_t;

using datapair_t = std::pair<jsn::data_t, jsn::data_t>;
using valuetuple_t = std::tuple<jsn::key_t const&, jsn::value_t const&, jsn::value_t const&>;

struct return_pair {
  return_pair(datapair_t& pair) : data{pair.first}, metadata{pair.second} {}

  template <typename T>
  T& data_() {
    return boost::get<T>(data.value);
  }

  template <typename T>
  T& metadata_() {
    return boost::get<T>(metadata.value);
  }

  jsn::data_t& data;
  jsn::data_t& metadata;
};

struct extra_opts {
  bool readProlog = false;
  bool readMain = true;

  inline void enablePrologMode() {
    readProlog = true;
    readMain = false;
  }
  inline void enableDefaultMode() {
    readProlog = false;
    readMain = true;
  }
  inline void enableCombinedMode() {
    readProlog = true;
    readMain = true;
  }
};

struct fcl2jsondb final {
  using fcl_value = ::fhicl::extended_value;
  using fhicl_key_value_pair_t = std::pair<const std::string, fcl_value> const;
  using args_tuple_t =
      std::tuple<fhicl_key_value_pair_t const&, fhicl_key_value_pair_t const&, comments_t const&, extra_opts const&>;

  explicit fcl2jsondb(args_tuple_t);

  operator datapair_t();

  fhicl_key_value_pair_t const& self;
  fhicl_key_value_pair_t const& parent;
  comments_t const& comments;
  extra_opts const& opts;
};

struct json2fcldb final {
  using args_tuple_t = std::tuple<valuetuple_t const&, valuetuple_t const&, extra_opts const&>;

  explicit json2fcldb(args_tuple_t);

  operator fcl::value_t();
  operator fcl::atom_t();

  valuetuple_t const& self;
  valuetuple_t const& parent;
  extra_opts const& opts;
};

namespace debug {
void FCL2JSON();
}

}  // namespace fhicljson
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_DATAFORMATS_FHICL_CONVERTFHICL2JSONDB_H_ */
