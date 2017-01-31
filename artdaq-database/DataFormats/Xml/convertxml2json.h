#ifndef _ARTDAQ_DATABASE_DATAFORMATS_XML_CONVERTXML2JSON_H_
#define _ARTDAQ_DATABASE_DATAFORMATS_XML_CONVERTXML2JSON_H_

#include "artdaq-database/DataFormats/common.h"

#include "artdaq-database/DataFormats/Json/json_types.h"
#include "artdaq-database/DataFormats/Xml/xml_types.h"

namespace artdaq {
namespace database {
namespace xmljson {

namespace jsn = artdaq::database::json;

struct xml2json final {};

struct json2xml final {};

namespace debug {
void Xml2Json();
}
}  // namespace xmljson
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_DATAFORMATS_XML_CONVERTXML2JSON_H_ */
