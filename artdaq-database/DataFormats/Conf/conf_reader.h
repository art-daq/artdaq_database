#ifndef _ARTDAQ_DATABASE_DATAFORMATS_CONF_XML_READER_H_
#define _ARTDAQ_DATABASE_DATAFORMATS_CONF_XML_READER_H_

#include "artdaq-database/DataFormats/common.h"

#include "artdaq-database/DataFormats/Json/json_types.h"
#include "artdaq-database/DataFormats/Conf/conf_types.h"

namespace artdaq {
namespace database {
namespace conf {

namespace fcl = artdaq::database::conf;
namespace jsn = artdaq::database::json;

struct ConfReader final {
  bool read(std::string const&, jsn::object_t&);
};

namespace debug {
void enableConfReader();
}
}  // namespace conf
}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_DATAFORMATS_CONF_XML_READER_H_ */
