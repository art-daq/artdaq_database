#ifndef _ARTDAQ_DATABASE_FHICLJSON_CONVERTFHICL2JSONDB_H_
#define _ARTDAQ_DATABASE_FHICLJSON_CONVERTFHICL2JSONDB_H_

#include "artdaq-database/FhiclJson/common.h"

#include "artdaq-database/FhiclJson/fhicl_types.h"
#include "artdaq-database/FhiclJson/json_types.h"
#include "fhiclcpp/extended_value.h"

namespace artdaq{
namespace database{
namespace fhicljson{

namespace fcl = artdaq::database::fhicl;
namespace jsn = artdaq::database::json;
  
using comment_t = std::string;
using linenum_t = int;
using comments_t = std::map<linenum_t, comment_t>;

using datapair_t = std::pair<jsn::data_t,jsn::data_t>;
using valuetuple_t = std::tuple<jsn::key_t const&, jsn::value_t const&,jsn::value_t const&>;


struct return_pair{
  return_pair(datapair_t& pair)
  :data{pair.first},metadata{pair.second}{}
  
  template<typename T>  
  T& data_(){
    return boost::get<T>(data.value);
  }
  
  template<typename T>  
  T& metadata_(){
    return boost::get<T>(metadata.value);
  }
  
  jsn::data_t& data;
  jsn::data_t& metadata;
};

struct fcl2jsondb final {
    using fcl_value = ::fhicl::extended_value;
    using fhicl_key_value_pair_t = std::pair<std::string, fcl_value> const;
    
    explicit fcl2jsondb(fhicl_key_value_pair_t const&,
                      fhicl_key_value_pair_t const&,
                      comments_t const&);

    operator datapair_t ();

    fhicl_key_value_pair_t const& self;
    fhicl_key_value_pair_t const& parent;
    comments_t const& comments;
};



struct json2fcldb final {
    explicit json2fcldb(valuetuple_t const&,
                      valuetuple_t const&);
    
    operator fcl::value_t();
    operator fcl::atom_t();

    valuetuple_t const& self;
    valuetuple_t const& parent;
};

void  trace_enable_fcl2jsondb();

} //namespace fhicljson
} //namespace database
} //namespace artdaq

#endif /* _ARTDAQ_DATABASE_FHICLJSON_CONVERTFHICL2JSONDB_H_ */
