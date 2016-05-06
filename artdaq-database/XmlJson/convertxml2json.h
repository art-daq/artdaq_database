#ifndef _ARTDAQ_DATABASE_XMLJSON_CONVERTXML2JSON_H_
#define _ARTDAQ_DATABASE_XMLJSON_CONVERTXML2JSON_H_

#include "artdaq-database/XmlJson/common.h"

#include "artdaq-database/FhiclJson/json_types.h"
#include "artdaq-database/XmlJson/xml_types.h"

namespace artdaq {
namespace database {
namespace xmljson {

namespace jsn = artdaq::database::json;

struct xml2json final {};

struct json2xml final {};

void trace_enable_xml2json();

}  // namespace xmljson
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_XMLJSON_CONVERTXML2JSON_H_ */
