#ifndef _ARTDAQ_DATABASE_FHICLJSON_FHICL2JSONGUI_H_
#define _ARTDAQ_DATABASE_FHICLJSON_FHICL2JSONGUI_H_

#include <string>

namespace artdaq{
namespace database{
namespace fhicljsongui{
bool fhicl_to_json(std::string const&, std::string&);
bool json_to_fhicl(std::string const&, std::string&);
} //namespace fhicljson

bool json_db_to_gui(std::string const&, std::string&);
bool json_gui_to_db(std::string const&, std::string&);

} //namespace database
} //namespace artdaq
#endif /* _ARTDAQ_DATABASE_FHICLJSON_FHICL2JSONGUI_H_ */
