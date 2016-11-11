#ifndef _ARTDAQ_DATABASE_DATAFORMATS_BIN_CONVERTBIN2JSON_H_
#define _ARTDAQ_DATABASE_DATAFORMATS_BIN_CONVERTBIN2JSON_H_

#include "artdaq-database/DataFormats/common.h"

#include "artdaq-database/DataFormats/Json/json_types.h"
#include "artdaq-database/DataFormats/Bin/bin_types.h"

namespace artdaq {
namespace database {
namespace binjson {

namespace jsn = artdaq::database::json;

struct bin2json final {};

struct json2bin final {};

namespace debug {
void enableBin2Json();
}
}  // namespace binjson
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_DATAFORMATS_BIN_CONVERTBIN2JSON_H_ */
