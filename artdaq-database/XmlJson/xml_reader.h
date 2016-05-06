#ifndef _ARTDAQ_DATABASE_XMLJSON_XML_READER_H_
#define _ARTDAQ_DATABASE_XMLJSON_XML_READER_H_

#include "artdaq-database/XmlJson/common.h"

#include "artdaq-database/FhiclJson/json_types.h"
#include "artdaq-database/XmlJson/xml_types.h"

namespace artdaq {
namespace database {
namespace xml {

namespace fcl = artdaq::database::xml;
namespace jsn = artdaq::database::json;

struct XmlReader final {
  bool read(std::string const&, jsn::object_t&);
};

void trace_enable_XmlReader();

}  // namespace xml
}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_XMLJSON_XML_READER_H_ */
