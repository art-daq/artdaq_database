#include "artdaq-database/SharedCommon/common.h"
#include "artdaq-database/SharedCommon/configuraion_api_literals.h"
#include "artdaq-database/SharedCommon/helper_functions.h"
#include "artdaq-database/SharedCommon/shared_exceptions.h"

#include <sys/utsname.h>
#include <wordexp.h>
#include <clocale>
#include <ctime>
#include <fstream>
#include <regex>

namespace db = artdaq::database;
using namespace artdaq::database;

namespace apiliteral = db::configapi::literal;

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

void db::set_default_locale() { std::setlocale(LC_ALL, apiliteral::database_format_locale); }

std::string db::timestamp() {
  auto now = std::chrono::system_clock::now();
  std::time_t time = std::chrono::system_clock::to_time_t(now);
  char buff[100];
  std::strftime(buff, sizeof(buff), apiliteral::timestamp_format, std::localtime(&time));

  if (useFakeTime()) return apiliteral::timestamp_faketime;

  return {buff};
}

std::string db::unamejson() {
  struct utsname thisuname;
  if (uname(&thisuname) == -1) {
    return "{}";
  } else {
    std::ostringstream oss;
    oss << "{";
    oss << "sysname"_quoted
        << ":" << quoted_(thisuname.sysname) << ",";
    oss << "nodename"_quoted
        << ":" << quoted_(thisuname.nodename) << ",";
    oss << "release"_quoted
        << ":" << quoted_(thisuname.release) << ",";
    oss << "version"_quoted
        << ":" << quoted_(thisuname.version) << ",";
    oss << "machine"_quoted
        << ":" << quoted_(thisuname.machine);
    oss << "}";
    return oss.str();
  }
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

std::string db::dequote(std::string s) {
  if (s[0] == '\"' && s[s.length() - 1] == '\"')
    return s.substr(1, s.length() - 2);
  else
    return s;
}

std::string db::debracket(std::string s) {
  if (s[0] == '[' && s[s.length() - 1] == ']')
    return s.substr(1, s.length() - 2);
  else
    return s;
}

std::string db::annotate(std::string const& s){
  auto str=db::trim(s);
  
  if(str[0] == '#' || str.empty())
    return str;
  
  return std::string{"#"}.append(str);
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
  oss << "{"
      << "_id"_quoted
      << ":{";
  oss << "$oid"_quoted
      << ":" << quoted_(oid);
  oss << "} }";

  return oss.str();
}

std::string db::to_json(std::string const& key, std::string const& value) {
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

  auto result = oss.str();

  if (result.back() == '/') result.pop_back();  // remove trailing slash

  return result;
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

  if (!std::regex_search(filter, results, ex))
    throw std::logic_error(std::string("Regex ouid search failed; JSON buffer:") + filter);

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

std::string db::trim(std::string const& s) {
  auto wsfront = std::find_if_not(s.begin(), s.end(), [](int c) { return std::isspace(c); });
  return std::string(wsfront, std::find_if_not(s.rbegin(), std::string::const_reverse_iterator(wsfront), [](int c) {
                                return std::isspace(c);
                              }).base());
}

std::string db::to_lower(std::string const& c) {
  auto s = c;
  std::transform(s.begin(), s.end(), s.begin(), ::tolower);
  return s;
}

std::string db::to_upper(std::string const& c) {
  auto s = c;
  std::transform(s.begin(), s.end(), s.begin(), ::toupper);
  return s;
}
