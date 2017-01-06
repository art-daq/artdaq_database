#include <cassert>
#include <chrono>
#include <regex>
#include <string>

#include "artdaq-database/DataFormats/common/helper_functions.h"

namespace db = artdaq::database;
using namespace artdaq::database;

std::string db::filter_jsonstring(std::string const& str) {
  assert(!str.empty());

  if (str.empty()) throw std::invalid_argument("Failed calling filter_jsonstring(): Invalid str; str is empty");

  auto retValue = std::string{str.c_str()};

  retValue = std::regex_replace(retValue, std::regex("\""), "|");
  retValue = std::regex_replace(retValue, std::regex("\'"), "^");
  retValue = std::regex_replace(retValue, std::regex("\t"), "    ");

  return retValue;
}

bool db::useFakeTime(bool useFakeTime = false) {
  static bool _useFakeTime = useFakeTime;
  return _useFakeTime;
}

std::string db::timestamp() {
  auto now = std::chrono::system_clock::now();
  std::time_t time = std::chrono::system_clock::to_time_t(now);
  auto result = std::string(std::ctime(&time));
  result.pop_back();

  if (useFakeTime()) return "Mon Feb  8 14:00:30 2016";

  return result;
}

std::string db::quoted_(std::string const& text) { return "\"" + text + "\""; }

std::string db::quoted_(bool const& value) { return (value ? "true" : "false"); }

std::string db::operator"" _quoted(const char* text, std::size_t) { return "\"" + std::string(text) + "\""; }

std::string db::debrace(std::string s) {
  if (s[0] == '{' && s[s.length() - 1] == '}')
    return s.substr(1, s.length() - 2);
  else
    return s;
}
