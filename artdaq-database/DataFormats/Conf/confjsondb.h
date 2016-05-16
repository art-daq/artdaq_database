#ifndef _ARTDAQ_DATABASE_DATAFORMATS_CONF_CONF2JSONDB_H_
#define _ARTDAQ_DATABASE_DATAFORMATS_CONF_CONF2JSONDB_H_

#include <string>

namespace artdaq {
namespace database {

namespace confjson {
bool conf_to_json(std::string const&, std::string&);
bool json_to_conf(std::string const&, std::string&);
namespace debug {
void enableConfJson();
}
}  // namespace confjson

namespace conf {
namespace debug {
void enableConfReader();
void enableConfWriter();
}
}  // namespace conf

}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_DATAFORMATS_CONF_CONF2JSONDB_H_ */
