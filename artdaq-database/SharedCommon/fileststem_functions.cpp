#include <libgen.h>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

#include "artdaq-database/SharedCommon/common.h"

#include "artdaq-database/SharedCommon/helper_functions.h"
#include "artdaq-database/SharedCommon/fileststem_functions.h"

namespace db = artdaq::database;
using namespace artdaq::database;

std::vector<std::string> db::list_files(std::string const& path) {
  confirm(!path.empty());

  auto files = std::vector<std::string>{};
  files.reserve(1024);

  for (auto& entry : boost::make_iterator_range(boost::filesystem::directory_iterator(path), {})) {
    if (boost::filesystem::is_regular(entry))
      files.push_back(entry.path().string());
    else if (boost::filesystem::is_directory(entry.path())) {
      auto suddir_list = list_files(entry.path().string());
      std::copy(suddir_list.begin(), suddir_list.end(), std::back_inserter(files));
    }
  }
  
  return files;
}

std::string db::collection_name_from_relative_path(std::string const& file_path_str) {
  auto file_path_copy = std::string{file_path_str.c_str()};
  char* file_path = (char*)(file_path_copy.c_str());
  auto names = std::list<std::string>{};
  names.push_front(basename(file_path));
  char* parent = dirname(file_path);
  names.push_front(basename(parent));

  //  parent = dirname(parent);
  //  names.push_front(basename(parent));

  std::ostringstream oss;
  for (auto const& name : names) oss << name << ".";
  auto collName = oss.str();
  collName.pop_back();

  return collName;
}

std::string db::relative_path_from_collection_name(std::string const& collection_name) {
  confirm(!collection_name.empty());

  auto retValue = std::string{collection_name};
  auto begin = retValue.begin();
  auto end = retValue.begin();
  std::advance(end, retValue.rfind('.') - 1);
  std::replace(begin, end, '.', '/');

  return retValue;
}

bool db::mkdir(std::string const& path) {
  confirm(!path.empty());

  auto cmd = std::string{"mkdir -p "};
  cmd.append(path.c_str());

  return 0 == system(cmd.c_str());
}

bool db::mkdirfile(std::string const& file) {
  confirm(!file.empty());

  auto tmp = std::string{file.c_str()};

  return mkdir(dirname((char*)tmp.c_str()));
}
