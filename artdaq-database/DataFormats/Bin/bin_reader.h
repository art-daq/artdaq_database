#ifndef _ARTDAQ_DATABASE_DATAFORMATS_CONF_BIN_READER_H_
#define _ARTDAQ_DATABASE_DATAFORMATS_CONF_BIN_READER_H_

#include "artdaq-database/DataFormats/common.h"

#include "artdaq-database/DataFormats/Json/json_types.h"
#include "artdaq-database/DataFormats/Bin/bin_types.h"

namespace artdaq {
namespace database {
namespace bin {

namespace fcl = artdaq::database::bin;
namespace jsn = artdaq::database::json;

struct BinReader final {
  bool read(std::vector<unsigned char> const&, jsn::object_t&);
};

namespace debug {
void enableBinReader();
}
}  // namespace bin
}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_DATAFORMATS_CONF_BIN_READER_H_ */
