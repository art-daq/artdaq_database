#ifndef _ARTDAQ_DATABASE_CONFIGURATIONDB_SHARED_HELPER_FUNCTIONS_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONDB_SHARED_HELPER_FUNCTIONS_H_

#include <string>
#include <vector>

namespace artdaq {
namespace database {
namespace configuration {
namespace options {
enum struct data_format_t { unknown, json, fhicl, gui, db };
}

bool equal(std::string const&, std::string const&);
bool not_equal(std::string const&, std::string const&);
std::string quoted_(std::string const&);
std::string to_string(options::data_format_t const&);
options::data_format_t to_data_format(std::string const&);
std::string dequote(std::string);
std::vector<std::string> list_files(std::string const&);
std::string collection_name_from_relative_path(std::string const&);

}  // namespace configuration
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_CONFIGURATIONDB_SHARED_HELPER_FUNCTIONS_H_ */
