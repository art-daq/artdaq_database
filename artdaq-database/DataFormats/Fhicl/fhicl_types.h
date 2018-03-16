#ifndef _ARTDAQ_DATABASE_FHICLTYPES_H_
#define _ARTDAQ_DATABASE_FHICLTYPES_H_

#include "artdaq-database/DataFormats/common.h"
#include "artdaq-database/DataFormats/shared_types.h"
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

namespace artdaq {
namespace database {
namespace fhicl {

using namespace artdaq::database;

struct table_t;
struct sequence_t;

using variant_value_t = sharedtypes::variant_value_of<table_t, sequence_t>;

using sharedtypes::optional_comment_t;
using sharedtypes::optional_annotation_t;
using sharedtypes::basic_key_t;

using key_t = sharedtypes::key_of<basic_key_t, optional_comment_t>;
using value_t = sharedtypes::value_of<variant_value_t, optional_annotation_t>;
using atom_t = sharedtypes::kv_pair_of<key_t, value_t>;

struct table_t : sharedtypes::table_of<atom_t> {};
struct sequence_t : sharedtypes::vector_of<value_t> {};

}  // namespace fhicl
}  // namespace database
}  // namespace artdaq

namespace fcl = artdaq::database::fhicl;

BOOST_FUSION_ADAPT_STRUCT(fcl::atom_t, (fcl::key_t, key)(fcl::value_t, value))

BOOST_FUSION_ADAPT_STRUCT(fcl::key_t, (fcl::optional_comment_t, comment)(fcl::basic_key_t, key))

BOOST_FUSION_ADAPT_STRUCT(fcl::value_t, (fcl::variant_value_t, value)(fcl::optional_annotation_t, annotation))

#endif /* _ARTDAQ_DATABASE_FHICLTYPES_H_ */
