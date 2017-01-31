#ifndef _ARTDAQ_DATABASE_DATAFORMATS_XML_XML_READER_H_
#define _ARTDAQ_DATABASE_DATAFORMATS_XML_XML_READER_H_

#include "artdaq-database/DataFormats/common.h"

#include "artdaq-database/DataFormats/Json/json_types.h"
#include "artdaq-database/DataFormats/Xml/xml_types.h"

namespace artdaq {
namespace database {
namespace xml {

struct XmlReader final {
  bool read(std::string const&, jsn::object_t&);
};

namespace debug {
void XmlReader();
}
}  // namespace xml
}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_DATAFORMATS_XML_XML_READER_H_ */
