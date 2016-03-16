#include "artdaq-database/ConfigurationDB/common.h"

#include "artdaq-database/ConfigurationDB/dboperation_filedb.h"
#include "artdaq-database/ConfigurationDB/dboperation_findconfigs.h"
#include "artdaq-database/ConfigurationDB/dboperation_mongodb.h"

#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/FhiclJson/shared_literals.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/JsonDocument/JSONDocument_template.h"

#include <boost/exception/diagnostic_information.hpp>

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "CONF:OpSrch_C"

namespace db = artdaq::database;
namespace dbjsu = db::jsonutils;
namespace dbjsul = db::jsonutils::literal;
namespace cf = db::configuration;
namespace cfgui = cf::guiexports;
namespace cfo = cf::options;
namespace cfol = cfo::literal;

using cfo::FindConfigsOperation;
using cfo::data_format_t;

using artdaq::database::basictypes::JsonData;
using artdaq::database::basictypes::FhiclData;
using artdaq::database::jsonutils::JSONDocument;
using artdaq::database::jsonutils::JSONDocumentBuilder;

namespace artdaq {
namespace database {
bool json_db_to_gui(std::string const&, std::string&);
bool json_gui_to_db(std::string const&, std::string&);
}
}
namespace detail {
void find_global_configurations(FindConfigsOperation const&, std::string&);
void build_global_configuration_search_filter(FindConfigsOperation const&, std::string&);
}

void FindConfigsOperation::read(std::string const& search_filter) {
  assert(!search_filter.empty());

  auto filter_document = JSONDocument{search_filter};

  try {
    _search_filter = filter_document.value_as<std::string>(cfol::filter);
  } catch (...) {
  }
}

JSONDocument FindConfigsOperation::search_filter_jsondoc() const {
  if (_search_filter != cfol::notprovided) {
    return {_search_filter};
  }

  std::stringstream ss;
  ss << "{";

  ss << "}";

  return {ss.str()};
}

cf::result_pair_t cf::find_global_configurations(FindConfigsOperation const& options,
                                                 std::string& name_pattern[[gnu::unused]]) noexcept {
  try {
    auto configuraions = std::string{};

    detail::find_global_configurations(options, configuraions);
    return cf::result_pair_t{true, "Success"};
  } catch (...) {
    return cf::result_pair_t{false, boost::current_exception_diagnostic_information()};
  }
}

cf::result_pair_t cf::build_global_configuration_search_filter(FindConfigsOperation const& options,
                                                               std::string& global_configs) noexcept {
  try {
    auto result = std::string{};

    detail::build_global_configuration_search_filter(options, result);

    global_configs.swap(result);

    return cf::result_pair_t{true, global_configs};
  } catch (...) {
    return cf::result_pair_t{false, boost::current_exception_diagnostic_information()};
  }
}

cf::result_pair_t cfgui::find_global_configurations(std::string const& search_filter,
                                                    std::string const& name_pattern[[gnu::unused]]) noexcept {
  try {
    auto options = FindConfigsOperation{};
    options.read(search_filter);

    auto configuraions = std::string{};

    detail::find_global_configurations(options, configuraions);
    return cf::result_pair_t{true, configuraions};
  } catch (...) {
    return cf::result_pair_t{false, boost::current_exception_diagnostic_information()};
  }
}

cf::result_pair_t cfgui::build_global_configuration_search_filter(std::string const& search_filter,
                                                                  std::string& global_configs) noexcept {
  try {
    auto options = FindConfigsOperation{};
    options.read(search_filter);

    auto result = std::string{};

    detail::build_global_configuration_search_filter(options, result);

    global_configs.swap(result);

    return cf::result_pair_t{true, global_configs};
  } catch (...) {
    return cf::result_pair_t{false, boost::current_exception_diagnostic_information()};
  }
}

void cf::trace_enable_FindConfigsOperation() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", 1LL);
  TRACE_CNTL("modeS", 1LL);

  TRACE_(0, "artdaq::database::configuration::FindConfigsOperation"
                << "trace_enable");
}
