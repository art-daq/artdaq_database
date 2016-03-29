#include "artdaq-database/ConfigurationDB/common.h"

#include "artdaq-database/ConfigurationDB/dboperation_filedb.h"
#include "artdaq-database/ConfigurationDB/dboperation_loadstore.h"
#include "artdaq-database/ConfigurationDB/dboperation_createconfigs.h"
#include "artdaq-database/ConfigurationDB/dboperation_mongodb.h"

#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/FhiclJson/shared_literals.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/JsonDocument/JSONDocument_template.h"

#include <boost/exception/diagnostic_information.hpp>

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "CONF:OpCrGCon_C"

namespace db = artdaq::database;
namespace dbjsu = db::jsonutils;
namespace dbjsul = db::jsonutils::literal;
namespace cf = db::configuration;
namespace cfgui = cf::guiexports;
namespace cfo = cf::options;
namespace cfol = cfo::literal;

using cfo::LoadStoreOperation;
using cfo::data_format_t;

using namespace artdaq::database::jsonutils;

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
void add_configuration_to_global_configuration(LoadStoreOperation const&, std::string&);
void create_new_global_configuration(LoadStoreOperation const&, std::string&);
void find_configuration_versions(LoadStoreOperation const&, std::string&);
void find_configuration_entities(LoadStoreOperation const&, std::string&);
}

cf::result_pair_t cf::add_configuration_to_global_configuration(LoadStoreOperation const& options) noexcept {
  try {
    auto returnValue = std::string{};

    detail::add_configuration_to_global_configuration(options, returnValue);
    return cf::result_pair_t{true, returnValue};
  } catch (...) {
    return cf::result_pair_t{false, boost::current_exception_diagnostic_information()};
  }
}

cf::result_pair_t cf::create_new_global_configuration(LoadStoreOperation const& options) noexcept {
  try {
    auto returnValue = std::string{};

    detail::create_new_global_configuration(options, returnValue);

    return cf::result_pair_t{true, returnValue};
  } catch (...) {
    return cf::result_pair_t{false, boost::current_exception_diagnostic_information()};
  }
}

cf::result_pair_t cf::find_configuration_versions(LoadStoreOperation const& options) noexcept {
  try {
    auto returnValue = std::string{};

    detail::find_configuration_versions(options, returnValue);

    return cf::result_pair_t{true, returnValue};
  } catch (...) {
    return cf::result_pair_t{false, boost::current_exception_diagnostic_information()};
  }
}

cf::result_pair_t cf::find_configuration_entities(LoadStoreOperation const& options) noexcept {
  try {
    auto returnValue = std::string{};

    detail::find_configuration_entities(options, returnValue);

    return cf::result_pair_t{true, returnValue};
  } catch (...) {
    return cf::result_pair_t{false, boost::current_exception_diagnostic_information()};
  }
}


cf::result_pair_t cfgui::add_configuration_to_global_configuration(std::string const& search_filter) noexcept {
  try {
    auto options = LoadStoreOperation{};
    options.read(search_filter);

    auto returnValue = std::string{};

    detail::add_configuration_to_global_configuration(options, returnValue);
    return cf::result_pair_t{true, returnValue};
  } catch (...) {
    return cf::result_pair_t{false, boost::current_exception_diagnostic_information()};
  }
}

cf::result_pair_t cfgui::create_new_global_configuration(std::string const& search_filter) noexcept {
  try {
    auto options = LoadStoreOperation{};
    options.read(search_filter);

    auto returnValue = std::string{};

    detail::create_new_global_configuration(options, returnValue);

    return cf::result_pair_t{true, returnValue};
  } catch (...) {
    return cf::result_pair_t{false, boost::current_exception_diagnostic_information()};
  }
}

cf::result_pair_t cfgui::find_configuration_versions(std::string const& search_filter) noexcept {
  try {
    auto options = LoadStoreOperation{};
    options.read(search_filter);

    auto returnValue = std::string{};

    detail::find_configuration_versions(options, returnValue);

    return cf::result_pair_t{true, returnValue};
  } catch (...) {
    return cf::result_pair_t{false, boost::current_exception_diagnostic_information()};
  }
}

cf::result_pair_t cfgui::find_configuration_entities(std::string const& search_filter) noexcept {
  try {
    auto options = LoadStoreOperation{};
    options.read(search_filter);

    auto returnValue = std::string{};

    detail::find_configuration_entities(options, returnValue);

    return cf::result_pair_t{true, returnValue};
  } catch (...) {
    return cf::result_pair_t{false, boost::current_exception_diagnostic_information()};
  }
}

void cf::trace_enable_CreateConfigsOperation() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", 1LL);
  TRACE_CNTL("modeS", 1LL);

  TRACE_(0, "artdaq::database::configuration::CreateConfigsOperation"
                << "trace_enable");
}
