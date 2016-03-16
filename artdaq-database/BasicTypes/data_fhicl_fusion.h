#ifndef _ARTDAQ_DATABASE_BASICTYPES_FHICL_FUSION_H_
#define _ARTDAQ_DATABASE_BASICTYPES_FHICL_FUSION_H_

#include "artdaq-database/BasicTypes/data_fhicl.h"

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

namespace cfg = artdaq::database::basictypes;

BOOST_FUSION_ADAPT_STRUCT(cfg::FhiclData, (std::string, fhicl_buffer))

#endif /* _ARTDAQ_DATABASE_BASICTYPES_FHICL_FUSION_H_ */
