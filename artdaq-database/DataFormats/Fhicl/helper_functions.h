#ifndef _ARTDAQ_DATABASE_DATAFORMATS_FHICL_HEALPERFUNCTIONS_H_
#define _ARTDAQ_DATABASE_DATAFORMATS_FHICL_HEALPERFUNCTIONS_H_

#include "artdaq-database/DataFormats/Fhicl/fhiclcpplib_includes.h"
#include "artdaq-database/DataFormats/common.h"
#include "artdaq-database/DataFormats/common/shared_literals.h"

namespace artdaq {
namespace database {
namespace fhicl {
bool isDouble(std::string const& str);
std::string unescape(std::string const& str);

std::string tag_as_string(::fhicl::value_tag);
::fhicl::value_tag string_as_tag(std::string);

}  // namespace fhicl
}  // namespace database
}  // artdaq

#endif /* _ARTDAQ_DATABASE_DATAFORMATS_FHICL_HEALPERFUNCTIONS_H_ */
