#ifndef _ARTDAQ_DATABASE_BASICTYPES_BIN_FUSION_H_
#define _ARTDAQ_DATABASE_BASICTYPES_BIN_FUSION_H_

#include "artdaq-database/BasicTypes/data_bin.h"

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

namespace cfg = artdaq::database::basictypes;

BOOST_FUSION_ADAPT_STRUCT(cfg::BinData, (std::vector<unsigned char>, bin_buffer))

#endif /* _ARTDAQ_DATABASE_BASICTYPES_BIN_FUSION_H_ */
