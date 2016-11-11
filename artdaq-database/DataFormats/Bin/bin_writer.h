#ifndef _ARTDAQ_DATABASE_DATAFORMATS_CONF_XML_WRITER_H_
#define _ARTDAQ_DATABASE_DATAFORMATS_CONF_XML_WRITER_H_

#include "artdaq-database/DataFormats/common.h"

#include "artdaq-database/DataFormats/Json/json_types.h"
#include "artdaq-database/DataFormats/Bin/bin_types.h"

namespace artdaq {
namespace database {
namespace bin {

namespace fcl = artdaq::database::bin;
namespace jsn = artdaq::database::json;

struct BinWriter final {
  bool write(jsn::object_t const&, std::string&);
};

namespace debug {
void enableBinWriter();
}
}  // namespace bin
}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_DATAFORMATS_CONF_XML_WRITER_H_ */
