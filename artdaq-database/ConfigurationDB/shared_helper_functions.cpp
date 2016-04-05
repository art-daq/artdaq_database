#include <cassert>

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
