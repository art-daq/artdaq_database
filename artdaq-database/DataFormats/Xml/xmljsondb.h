#ifndef _ARTDAQ_DATABASE_DATAFORMATS_XML_XML2JSONDB_H_
#define _ARTDAQ_DATABASE_DATAFORMATS_XML_XML2JSONDB_H_

#include <string>

namespace artdaq {
namespace database {

namespace xmljson {
bool xml_to_json(std::string const&, std::string&);
bool json_to_xml(std::string const&, std::string&);
namespace debug {
void XmlJson();
}
}  // namespace xmljson

namespace xml {
namespace debug {
void XmlReader();
void XmlWriter();
}
}  // namespace xml

}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_DATAFORMATS_XML_XML2JSONDB_H_ */
