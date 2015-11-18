#ifndef _ARTDAQ_DATABASE_FHICLJSON_FHICL_WRITER_H_
#define _ARTDAQ_DATABASE_FHICLJSON_FHICL_WRITER_H_

#include "artdaq-database/FhiclJson/common.h"
#include "artdaq-database/FhiclJson/fhicl_json.h"

namespace artdaq{
namespace database{
namespace fhicljson{

struct FhiclWriter final {
  bool write(any_value_t const&, std::string&);
};

} //namespace fhicljson
} //namespace database
} //namespace artdaq
#endif /* _ARTDAQ_DATABASE_FHICLJSON_FHICL_WRITER_H_ */
