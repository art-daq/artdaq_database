#include <chrono>
#include <regex>
#include <string>

#include "artdaq-database/DataFormats/common/helper_functions.h"

namespace artdaq {
namespace database {
namespace dataformats {

std::string filter_quotes(std::string const& str) { return std::regex_replace(str, std::regex("\""), "|"); }

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
