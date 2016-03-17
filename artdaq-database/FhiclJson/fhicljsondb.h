#ifndef _ARTDAQ_DATABASE_FHICLJSON_FHICL2JSONDB_H_
#define _ARTDAQ_DATABASE_FHICLJSON_FHICL2JSONDB_H_

#include <string>

namespace artdaq {
namespace database {

namespace fhicljsondb {
bool fhicl_to_json(std::string const&, std::string&);
bool json_to_fhicl(std::string const&, std::string&);
void trace_enable_fhicljsondb();

}  // namespace fhicljson

namespace fhicljson {
void trace_enable_fcl2jsondb();
}  // namespace fhicljson

namespace fhicl {
void trace_enable_FhiclReader();
void trace_enable_FhiclWriter();
}  // namespace fhicl

}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_FHICLJSON_FHICL2JSONDB_H_ */
