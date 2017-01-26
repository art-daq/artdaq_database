#ifndef _ARTDAQ_DATABASE_FILESYSTEM_FUNCTIONS_H_
#define _ARTDAQ_DATABASE_FILESYSTEM_FUNCTIONS_H_

#include <list>
#include <string>
#include <vector>

#include "artdaq-database/SharedCommon/shared_datatypes.h"

namespace artdaq {
namespace database {
std::vector<std::string> list_files(std::string const&);

std::string collection_name_from_relative_path(std::string const&);
std::string relative_path_from_collection_name(std::string const&);

bool mkdir(std::string const&);
bool mkdirfile(std::string const&);

}  // namespace database
}  // artdaq

#endif /* _ARTDAQ_DATABASE_FILESYSTEM_FUNCTIONS_H_ */
