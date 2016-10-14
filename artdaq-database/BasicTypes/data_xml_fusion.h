#ifndef _ARTDAQ_DATABASE_BASICTYPES_XML_FUSION_H_
#define _ARTDAQ_DATABASE_BASICTYPES_XML_FUSION_H_

#include "artdaq-database/BasicTypes/data_xml.h"

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

namespace cfg = artdaq::database::basictypes;

BOOST_FUSION_ADAPT_STRUCT(cfg::XmlData, (std::string, xml_buffer))

#endif /* _ARTDAQ_DATABASE_BASICTYPES_XML_FUSION_H_ */
