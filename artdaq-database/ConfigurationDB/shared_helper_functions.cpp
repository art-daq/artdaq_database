#include <algorithm>
#include <cassert>
#include <sstream>

#include <cstdlib>
#include <libgen.h>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include "artdaq-database/ConfigurationDB/shared_helper_functions.h"

namespace cf = artdaq::database::configuration;

using artdaq::database::configuration::options::data_format_t;

bool cf::equal(std::string const& left, std::string const& right) {
  assert(!left.empty());
  assert(!right.empty());

  return left.compare(right) == 0;
}

bool cf::not_equal(std::string const& left, std::string const& right) { return !equal(left, right); }
std::string cf::quoted_(std::string const& text) { return "\"" + text + "\""; }

std::string cf::to_string(data_format_t const& f) {
  switch (f) {
    default:
    case data_format_t::unknown:
      return "unknown";

    case data_format_t::fhicl:
      return "fhicl";

    case data_format_t::json:
      return "json";

    case data_format_t::gui:
      return "gui";

    case data_format_t::db:
      return "db";
  }
}

data_format_t cf::to_data_format(std::string const& f) {
  assert(!f.empty());

  if (f.compare("fhicl") == 0) {
    return data_format_t::fhicl;
  } else if (f.compare("json") == 0) {
    return data_format_t::json;
  } else if (f.compare("gui") == 0) {
    return data_format_t::gui;
  } else if (f.compare("db") == 0) {
    return data_format_t::db;
  } else {
    return data_format_t::unknown;
  }
}

std::string cf::dequote(std::string s) {
  if (s[0] == '"' && s[s.length() - 1] == '"')
    return s.substr(1, s.length() - 2);
  else
    return s;
}

std::vector<std::string> cf::list_files(std::string const& path) {
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

std::string cf::collection_name_from_relative_path(std::string const& file_path_str) {
  auto file_path_copy = std::string{file_path_str.c_str()};
  char* file_path = (char*)(file_path_copy.c_str());
  auto names = std::list<std::string>{};
  names.push_front(basename(file_path));
  char* parent = dirname(file_path);
  names.push_front(basename(parent));

  //  parent = dirname(parent);
  //  names.push_front(basename(parent));

  std::stringstream ss;
  for (auto const& name : names) ss << name << ".";
  auto collName = ss.str();
  collName.pop_back();

  return collName;
}

std::string cf::relative_path_from_collection_name(std::string const& collection_name) {
  assert(!collection_name.empty());
  auto retValue = std::string{collection_name};
  auto begin = retValue.begin();
  auto end = retValue.begin();
  std::advance(end, retValue.rfind('.') - 1);
  std::replace(begin, end, '.', '/');

  return retValue;
}

bool cf::mkdir(std::string const& path) {
  assert(!path.empty());

  auto cmd = std::string{"mkdir -p "};
  cmd.append(path.c_str());
  
  return 0 == system(cmd.c_str());
}

bool cf::mkdirfile(std::string const& file) {
  assert(!file.empty());

  auto tmp = std::string{file.c_str()};
  
  return mkdir(dirname((char*)tmp.c_str()));
}
