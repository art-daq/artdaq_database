#ifndef _ARTDAQ_DATABASE_FHICLJSON_HEALPERFUNCTIONS_H_
#define _ARTDAQ_DATABASE_FHICLJSON_HEALPERFUNCTIONS_H_

#include "artdaq-database/FhiclJson/common.h"
#include "artdaq-database/FhiclJson/shared_literals.h"
#include "fhiclcpp/extended_value.h"

namespace artdaq {
namespace database {
namespace fhicljson {

std::string filter_quotes(std::string const& str);
bool isDouble(std::string const& str);
std::string unescape(std::string const& str);

std::string tag_as_string(::fhicl::value_tag);
::fhicl::value_tag string_as_tag(std::string);

std::string timestamp();
bool useFakeTime(bool);

}  // namespace fhicljson
}  // namespace database
}  // artdaq

#endif /* _ARTDAQ_DATABASE_FHICLJSON_HEALPERFUNCTIONS_H_ */
