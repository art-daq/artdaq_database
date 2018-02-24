#include <libgen.h>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

#include "artdaq-database/SharedCommon/common.h"

#include "artdaq-database/SharedCommon/configuraion_api_literals.h"
#include "artdaq-database/SharedCommon/fileststem_functions.h"
#include "artdaq-database/SharedCommon/helper_functions.h"
#include "artdaq-database/SharedCommon/shared_exceptions.h"

namespace db = artdaq::database;
using namespace artdaq::database;

namespace db = artdaq::database;
namespace apiliteral = db::configapi::literal;

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
  
  auto pos= retValue.rfind('.');
  
  if(pos==std::string::npos)
    return retValue;
    
  std::advance(end, pos - 1);
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

bool db::write_buffer_to_file(std::string const& buffer, std::string const& file_out_name) try {
  confirm(!file_out_name.empty());

  if (!mkdirfile(file_out_name))
    throw runtime_error("write_buffer_to_file") << "write_buffer_to_file: Unable to create a directory path for "
                                                   "writing a config file file="
                                                << file_out_name;

  std::ofstream os(file_out_name.c_str());
  std::copy(buffer.begin(), buffer.end(), std::ostream_iterator<char>(os));
  os.close();
  return true;
} catch (...) {
  std::ostringstream oss;
  oss << "Failed to write a data file, fiel=" << file_out_name << ";\n";
  oss << ::debug::current_exception_diagnostic_information();
  throw runtime_error(oss.str());
  return false;
}

bool db::read_buffer_from_file(std::string& buffer, std::string const& file_in_name) try {
  confirm(!file_in_name.empty());
  confirm(buffer.empty());

  std::ifstream is(file_in_name);

  if (!is.good()) {
    is.close();

    throw invalid_argument("read_buffer_from_file") << "Failed calling read_buffer_from_file(): Failed opening a file="
                                                    << file_in_name;
  }

  std::stringstream ss;
  ss << is.rdbuf();
  is.close();

  buffer = ss.str();

  return true;
} catch (...) {
  std::ostringstream oss;
  oss << "Failed to read a data file, file=" << file_in_name << ";\n";
  oss << ::debug::current_exception_diagnostic_information();
  throw runtime_error(oss.str());
  return false;
}

std::string db::make_temp_dir() {
  auto tmp_dir_name = std::string{apiliteral::tmpdirprefix};
  srand(time(NULL));
  tmp_dir_name.append(std::to_string(rand() % 9000000 + 1000000));

  auto system_cmd = std::string{"mkdir -p "};
  system_cmd += tmp_dir_name;

  if (0 != system(system_cmd.c_str()))
    throw runtime_error("make_temp_dir") << "make_temp_dir: Unable to create a temp directory; system_cmd="
                                         << system_cmd;

  return tmp_dir_name;
}

void db::delete_temp_dir(std::string const& tmp_dir_name) {
  confirm(!tmp_dir_name.empty());

  auto prefix = std::string{apiliteral::tmpdirprefix};
  auto first(std::begin(prefix)), last(std::end(prefix));

  confirm(std::equal(first, last, tmp_dir_name.begin()));

  auto system_cmd = std::string{"rm -rf "}.append(tmp_dir_name);

  if (0 != system(system_cmd.c_str()))
    throw runtime_error("delete_temp_dir") << "delete_temp_dir: Unable to delete a temp directory; system_cmd="
                                           << system_cmd;
}

std::string const& db::dir_to_tarbzip2base64(std::string const& tmp_dir_name, std::string const& bzip2base64) {
  confirm(!tmp_dir_name.empty());
  confirm(!bzip2base64.empty());

  if (!mkdirfile(bzip2base64))
    throw runtime_error("dir_to_tarbzip2base64") << "dir_to_tarbzip2base64: Unable to create a directory; path="
                                                 << bzip2base64;

  std::ostringstream oss;
  oss << "tar cjf - " << tmp_dir_name << "/* -C " << tmp_dir_name << " 2>/dev/null";
  oss << "|base64 --wrap=0 > " << bzip2base64;

  if (0 != system(oss.str().c_str())) {
    throw runtime_error("dir_to_tarbzip2base64")
        << "dir_to_tarbzip2base64: Unable to create a tar.bzip2 file; system_cmd=" << oss.str();
  }

  return bzip2base64;
}

std::string const& db::tarbzip2base64_to_dir(std::string const& bzip2base64, std::string const& tmp_dir_name) {
  confirm(!bzip2base64.empty());
  confirm(!tmp_dir_name.empty());

  if (!mkdir(tmp_dir_name))
    throw runtime_error("tarbzip2base64_to_dir") << "tarbzip2base64_to_dir: Unable to create a directory; path="
                                                 << tmp_dir_name;

  std::ostringstream oss;
  oss << "cat " << bzip2base64 << " |base64 -d |tar xjf - -C " << tmp_dir_name;

  if (0 != system(oss.str().c_str())) {
    throw runtime_error("tarbzip2base64_to_dir")
        << "tarbzip2base64_to_dir: Unable to unzip configuration files; system_cmd=" << oss.str();
  }

  return tmp_dir_name;
}

bool db::extract_collectionname_from_filename(std::string const& file_name, std::string& collection_name) {
  confirm(!file_name.empty());

  boost::filesystem::path p(file_name);

  if (p.extension().string() != apiliteral::dbexport_extension) return false;

  collection_name = p.stem().string();

  return true;
}