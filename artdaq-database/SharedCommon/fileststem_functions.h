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

bool write_buffer_to_file(std::string const& /*buffer*/, std::string const& /*file_out_name*/);
bool read_buffer_from_file(std::string& /*buffer*/, std::string const& /*file_in_name*/);

std::string make_temp_dir();
void delete_temp_dir(std::string const& /*tmp_dir_name*/);
bool extract_collectionname_from_filename(std::string const& /*file_name*/, std::string& /*collection_name*/);
std::string const& dir_to_tarbzip2base64(std::string const& /*tmp_dir_name*/, std::string const& /*bzip2base64*/);
std::string const& tarbzip2base64_to_dir(std::string const& /*bzip2base64*/, std::string const& /*tmp_dir_name*/);

}  // namespace database
}  // artdaq

#endif /* _ARTDAQ_DATABASE_FILESYSTEM_FUNCTIONS_H_ */
