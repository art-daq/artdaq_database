#ifndef _ARTDAQ_DATABASE_DATAFORMATS_XML_XML_WRITER_H_
#define _ARTDAQ_DATABASE_DATAFORMATS_XML_XML_WRITER_H_

#include "artdaq-database/DataFormats/common.h"

#include "artdaq-database/DataFormats/Json/json_types.h"
#include "artdaq-database/DataFormats/Xml/xml_types.h"

namespace artdaq {
namespace database {
namespace xml {

namespace fcl = artdaq::database::xml;
namespace jsn = artdaq::database::json;

struct XmlWriter final {
  bool write(jsn::object_t const&, std::string&);
};

namespace debug {
void enableXmlWriter();
}
}  // namespace xml
}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_DATAFORMATS_XML_XML_WRITER_H_ */
