#ifndef _ARTDAQ_DATABASE_DATAFORMATS_CONF_CONVERTCONF2JSON_H_
#define _ARTDAQ_DATABASE_DATAFORMATS_CONF_CONVERTCONF2JSON_H_

#include "artdaq-database/DataFormats/common.h"

#include "artdaq-database/DataFormats/Conf/conf_types.h"
#include "artdaq-database/DataFormats/Json/json_types.h"

namespace artdaq {
namespace database {
namespace confjson {

namespace jsn = artdaq::database::json;

struct conf2json final {};

struct json2conf final {};

namespace debug {
void Conf2Json();
}
}  // namespace confjson
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_DATAFORMATS_CONF_CONVERTCONF2JSON_H_ */
