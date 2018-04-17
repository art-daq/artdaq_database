#ifndef _ARTDAQ_DATABASE_SHARED_DATATYPES_H_
#define _ARTDAQ_DATABASE_SHARED_DATATYPES_H_

#include <string>
#include <utility>

namespace artdaq {
namespace database {
using path_t = std::string;
using string_pair_t = std::pair<std::string, std::string>;
using object_id_t = std::string;
using timestamp_t = std::string;
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_SHARED_DATATYPES_H_ */
