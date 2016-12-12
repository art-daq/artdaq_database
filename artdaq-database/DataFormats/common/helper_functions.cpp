#include <chrono>
#include <regex>
#include <string>

#include "artdaq-database/DataFormats/common/helper_functions.h"

namespace artdaq {
namespace database {
namespace dataformats {

std::string filter_jsonstring(std::string const& str) {
  auto retValue = std::string{str.c_str()};

  retValue = std::regex_replace(retValue, std::regex("\""), "|");
  retValue = std::regex_replace(retValue, std::regex("\'"), "^");
  retValue = std::regex_replace(retValue, std::regex("\t"), "    ");

  return retValue;
}

bool useFakeTime(bool useFakeTime = false) {
  static bool _useFakeTime = useFakeTime;
  return _useFakeTime;
}

std::string timestamp() {
  auto now = std::chrono::system_clock::now();
  std::time_t time = std::chrono::system_clock::to_time_t(now);
  auto result = std::string(std::ctime(&time));
  result.pop_back();

  if (useFakeTime()) return "Mon Feb  8 14:00:30 2016";

  return result;
}

}  // namespace fhicljson
}  // namespace database
}  // artdaq
