#include <fstream>
#include <sstream>
#include <streambuf>

#include <boost/exception/diagnostic_information.hpp>
#include "artdaq-database/ConfigurationDB/dboperation_createconfigs.h"
#include "artdaq-database/ConfigurationDB/dboperation_findconfigs.h"
#include "artdaq-database/ConfigurationDB/dboperation_loadstore.h"

#include "conftool.h"

auto make_error_msg = [](auto msg) { return std::string("{error:\"").append(msg).append(".\"}"); };

namespace gui = artdaq::database::configuration::guiexports;

result_pair_t load_configuration(std::string const& search_filter) {
  try {
    if (search_filter.empty()) return std::make_pair(false, make_error_msg("Search filter is empty"));

    auto json_result = std::string();

    auto result = gui::load_configuration(search_filter, json_result);

    if (result.first)
      return std::make_pair(true, json_result);
    else
      return result;

  } catch (...) {
    return std::make_pair(false, boost::current_exception_diagnostic_information());
  }
}

result_pair_t store_configuration(std::string const& search_filter, std::string const& json_document) {
  try {
    if (search_filter.empty()) return std::make_pair(false, make_error_msg("Search filter is empty"));

    if (json_document.empty()) return std::make_pair(false, make_error_msg("JSON document is empty"));

    auto result = gui::store_configuration(search_filter, json_document);

    return result;
  } catch (...) {
    return std::make_pair(false, boost::current_exception_diagnostic_information());
  }
}

result_pair_t find_global_configurations(std::string const& search_filter) {
  try {
    if (search_filter.empty()) return std::make_pair(false, make_error_msg("Search filter is empty"));

    auto result = gui::find_global_configurations(search_filter);

    return result;
  } catch (...) {
    return std::make_pair(false, boost::current_exception_diagnostic_information());
  }
}

result_pair_t build_global_configuration_search_filter(std::string const& search_filter) {
  try {
    if (search_filter.empty()) return std::make_pair(false, make_error_msg("Search filter is empty"));

    auto result = gui::build_global_configuration_search_filter(search_filter);

    return result;
  } catch (...) {
    return std::make_pair(false, boost::current_exception_diagnostic_information());
  }
}

result_pair_t add_configuration_to_global_configuration(std::string const& search_filter) {
  try {
    if (search_filter.empty()) return std::make_pair(false, make_error_msg("Search filter is empty"));

    auto result = gui::add_configuration_to_global_configuration(search_filter);

    return result;
  } catch (...) {
    return std::make_pair(false, boost::current_exception_diagnostic_information());
  }
}

result_pair_t create_new_global_configuration(std::string const& search_filter) {
  try {
    if (search_filter.empty()) return std::make_pair(false, make_error_msg("Search filter is empty"));

    auto result = gui::create_new_global_configuration(search_filter);

    return result;
  } catch (...) {
    return std::make_pair(false, boost::current_exception_diagnostic_information());
  }
}

result_pair_t find_configuration_versions(std::string const& search_filter) {
  try {
    if (search_filter.empty()) return std::make_pair(false, make_error_msg("Search filter is empty"));

    auto result = gui::find_configuration_versions(search_filter);

    return result;
  } catch (...) {
    return std::make_pair(false, boost::current_exception_diagnostic_information());
  }
}
