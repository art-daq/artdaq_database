#include "artdaq-database/SharedCommon/common.h"
#include "artdaq-database/SharedCommon/helper_functions.h"
#include "artdaq-database/SharedCommon/shared_exceptions.h"

#include <chrono>
#include <wordexp.h>
#include <fstream>
#include <regex>

namespace db = artdaq::database;
using namespace artdaq::database;

std::string db::filter_jsonstring(std::string const& str) {
  confirm(!str.empty());

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

std::string db::bool_(bool value) { return (value ? "true" : "false"); }

std::string db::operator"" _quoted(const char* text, std::size_t) { return "\"" + std::string(text) + "\""; }

std::string db::debrace(std::string s) {
  if (s[0] == '{' && s[s.length() - 1] == '}')
    return s.substr(1, s.length() - 2);
  else
    return s;
}

std::string db::dequote(std::string s){
  if (s[0] == '\"' && s[s.length() - 1] == '\"')
    return s.substr(1, s.length() - 2);
  else
    return s;
}

std::string db::generate_oid() {
  std::ifstream is("/proc/sys/kernel/random/uuid");

  std::string oid((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());

  oid.erase(std::remove(oid.begin(), oid.end(), '-'), oid.end());
  oid.resize(24);

  return oid;
}


std::string db::to_id(std::string const& oid) {
  confirm(!oid.empty());
  
  std::ostringstream oss;
  oss << "{" << "_id"_quoted << ":{";
  oss << "$oid"_quoted << ":" << quoted_(oid);
  oss << "} }";
  
  return oss.str();
}

std::string db::to_json(std::string const& key,std::string const& value ) {
  confirm(!key.empty());
  confirm(!value.empty());
  
  std::ostringstream oss;
  oss << "{" << quoted_(key) << ":" << quoted_(value) << "}"; 
  return oss.str();
}


std::string db::expand_environment_variables(std::string var) {
  wordexp_t p;
  char** w;

  ::wordexp(var.c_str(), &p, 0);

  w = p.we_wordv;

  std::ostringstream oss;

  for (size_t i = 0; i < p.we_wordc; i++) oss << w[i] << "/";

  ::wordfree(&p);

  return oss.str();
}

bool db::equal(std::string const& left, std::string const& right) {
  confirm(!left.empty());
  confirm(!right.empty());

  return left.compare(right) == 0;
}

bool db::not_equal(std::string const& left, std::string const& right) { return !equal(left, right); }

db::object_id_t db::extract_oid(std::string const& filter) {
  auto ex = std::regex("^\\{[^:]+:\\s+([\\s\\S]+)\\}$");

  auto results = std::smatch();

  if (!std::regex_search(filter, results, ex)) throw std::logic_error(std::string("Regex ouid search failed; JSON buffer:") + filter);

  if (results.size() != 2) {
    // we are interested in a second match
    TRACE_(12, "value()"
                   << "JSON regex_search() result count=" << results.size());
    for (auto const& result : results) {
      TRACE_(12, "value()"
                     << "JSON regex_search() result=" << result);
    }

    throw runtime_error(std::string("Regex search failed, regex_search().size()!=1; JSON buffer: ") + filter);
  }

  auto match = std::string(results[1]);

  match.erase(match.find_last_not_of(" \n\r\t") + 1);

  auto dequote = [](auto s) {

    if (s[0] == '"' && s[s.length() - 1] == '"')
      return s.substr(1, s.length() - 2);
    else
      return s;
  };

  match = dequote(match);

  TRACE_(12, "value()"
                 << "JSON regex_search() result=" << match);

  return match;
}
