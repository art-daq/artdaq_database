#ifndef _ARTDAQ_DATABASE_XMLJSON_XML_WRITER_H_
#define _ARTDAQ_DATABASE_XMLJSON_XML_WRITER_H_

#include "artdaq-database/XmlJson/common.h"

#include "artdaq-database/FhiclJson/json_types.h"
#include "artdaq-database/XmlJson/xml_types.h"

namespace artdaq {
namespace database {
namespace xml {

namespace fcl = artdaq::database::xml;
namespace jsn = artdaq::database::json;

struct XmlWriter final {
  bool write(jsn::object_t const&, std::string&);
};

void trace_enable_XmlWriter();

}  // namespace xml
}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_XMLJSON_XML_WRITER_H_ */
