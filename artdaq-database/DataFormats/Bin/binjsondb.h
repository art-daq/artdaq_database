#ifndef _ARTDAQ_DATABASE_DATAFORMATS_BIN_BIN2JSONDB_H_
#define _ARTDAQ_DATABASE_DATAFORMATS_BIN_BIN2JSONDB_H_

#include <string>

namespace artdaq {
namespace database {

namespace binjson {
bool bin_to_json(std::vector<unsigned char> const&, std::string&);
bool json_to_bin(std::string const&, std::vector<unsigned char>&);
namespace debug {
void enableBinJson();
}
}  // namespace binjson

namespace bin {
namespace debug {
void enableBinReader();
void enableBinWriter();
}
}  // namespace bin

}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_DATAFORMATS_BIN_BIN2JSONDB_H_ */
