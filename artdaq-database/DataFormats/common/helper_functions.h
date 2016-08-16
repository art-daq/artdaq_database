#ifndef _ARTDAQ_DATABASE_DATAFORMATS_COMMON_HEALPERFUNCTIONS_H_
#define _ARTDAQ_DATABASE_DATAFORMATS_COMMON_HEALPERFUNCTIONS_H_

#include <string>

namespace artdaq {
namespace database {
namespace dataformats {

std::string filter_jsonstring(std::string const& str);
std::string timestamp();
bool useFakeTime(bool);

}  // namespace dataformats
}  // namespace database
}  // artdaq

#endif /* _ARTDAQ_DATABASE_DATAFORMATS_FHICL_HEALPERFUNCTIONS_H_ */
