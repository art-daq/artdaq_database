#ifndef _ARTDAQ_DATABASE_DATAFORMATS_COMMON_HEALPERFUNCTIONS_H_
#define _ARTDAQ_DATABASE_DATAFORMATS_COMMON_HEALPERFUNCTIONS_H_

#include <cassert>
#include <iostream>
#include <iterator>
#include <memory>
#include <sstream>
#include <string>
#include <chrono>

#include "artdaq-database/SharedCommon/shared_datatypes.h"

namespace debug {
std::string getStackTrace();
}

#ifndef NDEBUG
template <typename T>
inline void confirm(std::unique_ptr<T> const& expr) {
  if (!expr) {
    std::cerr << "Failed Assertion:" << ::debug::getStackTrace();
  }
  assert(expr);
}

inline void confirm(bool expr) {
  if (!expr) {
    std::cerr << "Failed Assertion:" << ::debug::getStackTrace();
  }
  assert(expr);
}
#else
#include "artdaq-database/SharedCommon/shared_exceptions.h"
template <typename T>
inline void confirm(std::unique_ptr<T> const& expr) {
  if (!expr) {
    auto msg = ::debug::getStackTrace();
    std::cerr << "Failed Assertion:" << msg;
    throw artdaq::database::runtime_exception("Failed assertion") << ::debug::getStackTrace();
  }
}

inline void confirm(bool expr) {
  if (!expr) {
    auto msg = ::debug::getStackTrace();
    std::cerr << "Failed Assertion:" << msg;
    throw artdaq::database::runtime_exception("Failed assertion") << ::debug::getStackTrace();
  }
}
#endif

namespace artdaq {
namespace database {
using std::chrono::system_clock;

std::string filter_jsonstring(std::string const& str);
std::string timestamp();
std::string to_string(system_clock::time_point const& tp);
system_clock::time_point to_timepoint(std::string const& strtime);
std::string confirm_iso8601_timestamp(std::string const& strtime);

bool useFakeTime(bool);
std::string quoted_(std::string const& /*text*/);
std::string bool_(bool /*bool*/);
std::string operator"" _quoted(const char* /*text*/, std::size_t);
std::string debrace(std::string /*s*/);
std::string dequote(std::string /*s*/);
std::string debracket(std::string /*s*/);
std::string annotate(std::string const& /*s*/);

void set_default_locale();

bool equal(std::string const&, std::string const&);
bool not_equal(std::string const&, std::string const&);

std::string expand_environment_variables(std::string /*var*/);
object_id_t extract_oid(std::string const&);

std::string generate_oid();
std::string to_id(std::string const& /*oid*/);
std::string to_json(std::string const& /*key*/, std::string const& /*value*/);

std::string trim(std::string const& s);
std::string to_lower(std::string const& s);
std::string to_upper(std::string const& s);

std::string unamejson();

template <typename T>
std::string to_csv(T const& data) {
  auto retvalue = std::string{};

  if (data.empty()) return retvalue;

  std::ostringstream oss;
  std::copy(data.begin(), data.end(), std::ostream_iterator<std::string>(oss, ","));

  retvalue = oss.str();
  retvalue.pop_back();

  return retvalue;
}

template <typename T>
constexpr std::uint8_t static_cast_as_uint8_t(T const& t) {
  return static_cast<std::uint8_t>(t);
}

}  // namespace database
}  // artdaq

#endif /* _ARTDAQ_DATABASE_DATAFORMATS_FHICL_HEALPERFUNCTIONS_H_ */
