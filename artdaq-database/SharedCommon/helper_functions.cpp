#include "artdaq-database/SharedCommon/helper_functions.h"
#include "artdaq-database/SharedCommon/common.h"
#include "artdaq-database/SharedCommon/configuraion_api_literals.h"
#include "artdaq-database/SharedCommon/shared_exceptions.h"

#include <sys/utsname.h>
#include <wordexp.h>
#include <chrono>
#include <clocale>
#include <ctime>
#include <fstream>
#include <regex>

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "helper_functions.cpp"

namespace db = artdaq::database;
using namespace artdaq::database;

namespace apiliteral = db::configapi::literal;

bool db::useFakeTime(bool useFakeTime = false) {
  static bool _useFakeTime = useFakeTime;
  return _useFakeTime;
}

void db::set_default_locale() { std::setlocale(LC_ALL, apiliteral::database_format_locale); }

std::string db::timestamp() {
  auto now = std::chrono::steady_clock::now();
  return db::to_string(now);
}

std::time_t steady_clock_to_time_t(std::chrono::steady_clock::time_point const& tp) {
  using namespace std::chrono;
  static auto system_clock = system_clock::now();
  static auto steady_clock = steady_clock::now();

  return system_clock::to_time_t(system_clock + duration_cast<system_clock::duration>(tp - steady_clock));
}

std::chrono::steady_clock::time_point steady_clock_from_time_t(std::time_t const& t) {
  using namespace std::chrono;
  static auto system_clock = system_clock::now();
  static auto steady_clock = steady_clock::now();

  return {steady_clock + (system_clock::from_time_t(t) - system_clock)};
}

std::string db::to_string(std::chrono::steady_clock::time_point const& tp) {
  std::time_t time = steady_clock_to_time_t(tp);
  char buff[40];
  std::strftime(buff, sizeof(buff), apiliteral::timestamp_format, std::localtime(&time));
  auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()).count() % 1000;

#ifndef __clang__
#define FORMAT_DURATION_MILLISECONDS "%03ld"
#else  //__clang__
#define FORMAT_DURATION_MILLISECONDS "%03lld"
#endif  //__clang__

  snprintf(buff + 30, 5, FORMAT_DURATION_MILLISECONDS, milliseconds);

  // We know that we're only copying 3 out of 9 potential bytes, it's okay...
#ifndef __clang__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
#endif
  strncpy(buff + 20, buff + 30, 3);

#ifndef __clang__
#pragma GCC diagnostic pop
#endif

  if (useFakeTime()) {
    return apiliteral::timestamp_faketime;
  }

  return {buff};
}

std::chrono::steady_clock::time_point db::to_timepoint(std::string const& strtime) {
  confirm(!strtime.empty());
  if (strtime.empty()) {
    throw std::invalid_argument("Failed calling to_timepoint(): Invalid strtime; strtime is empty");
  }

  auto timeinfo = std::tm();
  auto milliseconds = std::chrono::milliseconds(atoi(strtime.substr(20, 3).c_str()));

  auto tmptime = strtime;
  tmptime.at(20) = '0';
  tmptime.at(21) = '0';
  tmptime.at(22) = '0';

  if (strptime(tmptime.c_str(), apiliteral::timestamp_format, &timeinfo) != nullptr) {
    timeinfo.tm_isdst = -1;
    return steady_clock_from_time_t(std::mktime(&timeinfo)) + milliseconds;
  }

  throw std::invalid_argument(std::string("Failed calling to_timepoint(): format mismatch; format=<") + apiliteral::timestamp_format +
                              ">, timestamp=<" + strtime + ">");
}

std::string db::confirm_iso8601_timestamp(std::string const& strtime) {
  confirm(!strtime.empty());

  if (strtime.empty()) {
    throw std::invalid_argument("Failed calling confirm_iso8601_timestamp(): Invalid strtime; strtime is empty");
  }

  if (strtime.at(0) == '2') {
    return strtime;
  }
  auto timeinfo = std::tm();
  if (strptime(strtime.c_str(), apiliteral::timestamp_format_old, &timeinfo) != nullptr) {
    timeinfo.tm_isdst = -1;
    return db::to_string(steady_clock_from_time_t(std::mktime(&timeinfo)));
  }
  throw std::invalid_argument(std::string("Failed calling confirm_iso8601_timestamp(): format mismatch; format=<") +
                              apiliteral::timestamp_format_old + ">, timestamp=<" + strtime + ">");
}

std::string db::unamejson() {
  struct utsname thisuname;
  if (uname(&thisuname) == -1) {
    return "{}";
  }
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

std::string db::quoted_(std::string const& text, const char qchar) {
  confirm((qchar == '\"' || qchar == '\''));
  return std::string{} + qchar + text + qchar;
}

std::string db::bool_(bool value) { return (value ? "true" : "false"); }

std::string db::operator"" _quoted(const char* text, std::size_t /*unused*/) { return "\"" + std::string(text) + "\""; }

std::string db::debrace(std::string s) {
  if (s[0] == '{' && s[s.length() - 1] == '}') {
    return s.substr(1, s.length() - 2);
  }
  { return s; }
}

db::quotation_type_t db::quotation_type(std::string s) {
  if (s[0] == '\"' && s[s.length() - 1] == '\"') {
    return db::quotation_type_t::DOUBLE;
  }
  if (s[0] == '\'' && s[s.length() - 1] == '\'') {
    return db::quotation_type_t::SINGLE;
  }
  return db::quotation_type_t::NONE;
}

std::string db::dequote(std::string s) {
  if ((s[0] == '\"' && s[s.length() - 1] == '\"') || (s[0] == '\'' && s[s.length() - 1] == '\'')) {
    return s.substr(1, s.length() - 2);
  }
  { return s; }
}

std::string db::debracket(std::string s) {
  if (s[0] == '[' && s[s.length() - 1] == ']') {
    return s.substr(1, s.length() - 2);
  }
  { return s; }
}

std::string db::annotate(std::string const& s) {
  auto str = db::trim(s);

  if (str[0] == '#') {
    return str;
  }

  if (str.empty()) {
    return apiliteral::nullstring;
  }

  return std::string{"#"}.append(str);
}

std::string db::generate_oid() {
  TLOG(11) << "generate_oid() begin";

  std::ifstream is("/proc/sys/kernel/random/uuid");

  std::string oid((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());

  oid.erase(std::remove(oid.begin(), oid.end(), '-'), oid.end());
  oid.resize(24);

  TLOG(12) << "generate_oid() end";

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

std::string db::expand_environment_variables(const std::string& var) {
  wordexp_t p;
  char** w;

  ::wordexp(var.c_str(), &p, 0);

  w = p.we_wordv;

  std::ostringstream oss;

  for (size_t i = 0; i < p.we_wordc; i++) {
    oss << w[i] << "/";
  }

  ::wordfree(&p);

  auto result = oss.str();

  if (result.back() == '/') {
    result.pop_back();  // remove trailing slash
  }

  return result;
}

bool db::equal(std::string const& left, std::string const& right) {
  confirm(!left.empty());
  confirm(!right.empty());

  return left == right;
}

bool db::not_equal(std::string const& left, std::string const& right) { return !equal(left, right); }

db::object_id_t db::extract_oid(std::string const& filter) {
  auto ex = std::regex(R"(^\{[^:]+:\s+([\s\S]+)\}$)");

  auto results = std::smatch();

  if (!std::regex_search(filter, results, ex)) {
    throw std::logic_error(std::string("Regex ouid search failed; JSON buffer:") + filter);
  }

  if (results.size() != 2) {
    // we are interested in a second match
    TLOG(13) << "value() JSON regex_search() result count=" << results.size();
    for (auto const& result : results) {
      TLOG(14) << "value() JSON regex_search() result=" << result;
    }

    throw runtime_error(std::string("Regex search failed, regex_search().size()!=1; JSON buffer: ") + filter);
  }

  auto match = std::string(results[1]);

  match.erase(match.find_last_not_of(" \n\r\t") + 1);

  auto dequote = [](auto s) {
    if (s[0] == '"' && s[s.length() - 1] == '"') {
      return s.substr(1, s.length() - 2);
    }
    { return s; }
  };

  match = dequote(match);

  TLOG(15) << "value() JSON regex_search() result=" << match;

  return match;
}

std::string db::trim(std::string const& s) {
  auto wsfront = std::find_if_not(s.begin(), s.end(), [](int c) { return std::isspace(c); });
  return std::string(wsfront,
                     std::find_if_not(s.rbegin(), std::string::const_reverse_iterator(wsfront), [](int c) { return std::isspace(c); }).base());
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

std::string db::replace_all(std::string const& source, std::string const& match, std::string const& replacement) {
  auto retValue = std::string{};
  auto match_len = match.size();
  retValue.reserve(match_len);

  std::size_t last, first = 0;

  while (std::string::npos != (last = source.find(match, first))) {
    retValue.append(source, first, last - first);
    retValue.append(replacement);
    first = last + match_len;
  }

  retValue.append(source, first, std::string::npos);

  return retValue;
}

namespace artdaq {
namespace database {
template <>
std::string quoted<quotation_type_t::NONE>(std::string const& text) {
  return text;
}

template <>
std::string quoted<quotation_type_t::SINGLE>(std::string const& text) {
  return quoted_(text, '\'');
}
}  // namespace database
}  // namespace artdaq
