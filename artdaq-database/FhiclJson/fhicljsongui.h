#ifndef _ARTDAQ_DATABASE_FHICLJSON_FHICL2JSONGUI_H_
#define _ARTDAQ_DATABASE_FHICLJSON_FHICL2JSONGUI_H_

#include <string>

namespace artdaq{
namespace database{
namespace fhicljsongui{
bool fhicl_to_json(std::string const&, std::string&);
bool json_to_fhicl(std::string const&, std::string&);
} //namespace fhicljson
} //namespace database
} //namespace artdaq
#endif /* _ARTDAQ_DATABASE_FHICLJSON_FHICL2JSONGUI_H_ */
