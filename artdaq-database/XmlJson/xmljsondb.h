#ifndef _ARTDAQ_DATABASE_XMLJSON_XML2JSONDB_H_
#define _ARTDAQ_DATABASE_XMLJSON_XML2JSONDB_H_

#include <string>

namespace artdaq {
namespace database {

namespace xmljson {
bool xml_to_json(std::string const&, std::string&);
bool json_to_xml(std::string const&, std::string&);
void trace_enable_xmljsondb();

}  // namespace xmljson

namespace xml {
void trace_enable_FhiclReader();
void trace_enable_FhiclWriter();
}  // namespace xml

}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_XMLJSON_XML2JSONDB_H_ */
