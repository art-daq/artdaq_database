#ifndef _ARTDAQ_DATABASE_DATAFORMATS_COMMON_HEALPERFUNCTIONS_H_
#define _ARTDAQ_DATABASE_DATAFORMATS_COMMON_HEALPERFUNCTIONS_H_

#include <string>
#include <cassert>

#include "artdaq-database/SharedCommon/shared_datatypes.h"

# define confirm(expr) assert(expr)


namespace artdaq {
namespace database {
std::string filter_jsonstring(std::string const& str);
std::string timestamp();
bool useFakeTime(bool);
std::string quoted_(std::string const& /*text*/);
std::string bool_(bool /*bool*/);
std::string operator"" _quoted(const char* /*text*/, std::size_t);
std::string debrace(std::string /*s*/);
std::string dequote(std::string /*s*/);

bool equal(std::string const&, std::string const&);
bool not_equal(std::string const&, std::string const&);

std::string expand_environment_variables(std::string /*var*/);
object_id_t extract_oid(std::string const&);

std::string generate_oid();

template <typename T>
constexpr std::uint8_t static_cast_as_uint8_t(T const& t) {
  return static_cast<std::uint8_t>(t);
}

}  // namespace database
}  // artdaq

#endif /* _ARTDAQ_DATABASE_DATAFORMATS_FHICL_HEALPERFUNCTIONS_H_ */
