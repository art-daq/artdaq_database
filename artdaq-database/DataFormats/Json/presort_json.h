#ifndef _ARTDAQ_DATABASE_DATAFORMATS_JSON_PRESORTJSON_H_
#define _ARTDAQ_DATABASE_DATAFORMATS_JSON_PRESORTJSON_H_

#include <string>
#include <string_view>

namespace artdaq {
namespace database {
namespace json {

/// @brief Sorts the "search" array entries in a GUI JSON document by their "name" field.
/// @param json The input JSON string containing a "search" array
/// @return The JSON string with the "search" array sorted by "name", or the original if no "search" array found
[[nodiscard]] std::string presort_gui_json_by_name(std::string_view json);

}  // namespace json
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_DATAFORMATS_JSON_PRESORTJSON_H_ */
