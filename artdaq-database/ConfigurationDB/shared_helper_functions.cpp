#include <libgen.h>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

#include "artdaq-database/ConfigurationDB/shared_helper_functions.h"
#include "artdaq-database/SharedCommon/sharedcommon_common.h"

namespace cf = artdaq::database::configuration;

using artdaq::database::configuration::options::data_format_t;

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

    case data_format_t::xml:
      return "xml";

    case data_format_t::origin:
      return "origin";

    case data_format_t::csv:
      return "csv";
  }
}

data_format_t cf::to_data_format(std::string const& f) {
  confirm(!f.empty());

  if (f.compare("fhicl") == 0) {
    return data_format_t::fhicl;
  } else if (f.compare("json") == 0) {
    return data_format_t::json;
  } else if (f.compare("gui") == 0) {
    return data_format_t::gui;
  } else if (f.compare("db") == 0) {
    return data_format_t::db;
  } else if (f.compare("xml") == 0) {
    return data_format_t::xml;
  } else if (f.compare("origin") == 0) {
    return data_format_t::origin;
  } else if (f.compare("csv") == 0) {
    return data_format_t::csv;
  }

  return data_format_t::unknown;
}
