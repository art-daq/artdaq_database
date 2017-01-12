#ifndef _ARTDAQ_DATABASE_CONFIGURATIONDB_SHARED_HELPER_FUNCTIONS_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONDB_SHARED_HELPER_FUNCTIONS_H_

#include <string>
#include <vector>

namespace artdaq {
namespace database {
namespace configuration {
namespace options {
enum struct data_format_t { unknown, origin, json, fhicl, gui, db, xml, csv };
}

std::string to_string(options::data_format_t const&);
options::data_format_t to_data_format(std::string const&);
}  // namespace configuration
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_CONFIGURATIONDB_SHARED_HELPER_FUNCTIONS_H_ */
