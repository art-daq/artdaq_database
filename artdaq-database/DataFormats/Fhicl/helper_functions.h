#ifndef _ARTDAQ_DATABASE_DATAFORMATS_FHICL_HEALPERFUNCTIONS_H_
#define _ARTDAQ_DATABASE_DATAFORMATS_FHICL_HEALPERFUNCTIONS_H_

#include "artdaq-database/DataFormats/Fhicl/fhiclcpplib_includes.h"
#include "artdaq-database/DataFormats/common.h"

namespace artdaq {
namespace database {
namespace fhicl {
bool isDouble(std::string const& str);
std::string unescape(std::string const& str);

std::string tag_as_string(::fhicl::value_tag);
::fhicl::value_tag string_as_tag(std::string);

std::string protection_as_string(::fhicl::Protection);
::fhicl::Protection string_as_protection(std::string);

std::string to_json_string(std::string const& str);
std::string from_json_string(std::string const& str);

}  // namespace fhicl
}  // namespace database
}  // artdaq

#endif /* _ARTDAQ_DATABASE_DATAFORMATS_FHICL_HEALPERFUNCTIONS_H_ */
