#ifndef _ARTDAQ_DATABASE_DATAFORMATS_COMMON_HEALPERFUNCTIONS_H_
#define _ARTDAQ_DATABASE_DATAFORMATS_COMMON_HEALPERFUNCTIONS_H_

#include <string>

namespace artdaq {
namespace database {
std::string filter_jsonstring(std::string const& str);
std::string timestamp();
bool useFakeTime(bool);
std::string quoted_(std::string const& /*text*/);
std::string bool_(bool /*bool*/);
std::string operator"" _quoted(const char* /*text*/, std::size_t);
std::string debrace(std::string /*s*/);

std::string generate_oid();
}  // namespace database
}  // artdaq

#endif /* _ARTDAQ_DATABASE_DATAFORMATS_FHICL_HEALPERFUNCTIONS_H_ */
