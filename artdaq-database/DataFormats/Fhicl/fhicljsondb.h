#ifndef _ARTDAQ_DATABASE_DATAFORMATS_FHICL_FHICL2JSONDB_H_
#define _ARTDAQ_DATABASE_DATAFORMATS_FHICL_FHICL2JSONDB_H_

#include <string>

namespace artdaq {
namespace database {
namespace fhicljson {
bool fhicl_to_json(std::string const&, std::string&);
bool json_to_fhicl(std::string const&, std::string&);
namespace debug {
void enableFhiclJson();
void enableFCL2JSONDB();
}
}  // namespace fhicljson

namespace fhicl {
namespace debug {
void enableFhiclReader();
void enableFhiclWriter();
}
}  // namespace fhicl

}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_DATAFORMATS_FHICL_FHICL2JSONDB_H_ */
