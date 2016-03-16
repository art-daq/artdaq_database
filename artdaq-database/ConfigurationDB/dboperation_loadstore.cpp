#include "artdaq-database/ConfigurationDB/common.h"

#include "artdaq-database/ConfigurationDB/dboperation_filedb.h"
#include "artdaq-database/ConfigurationDB/dboperation_loadstore.h"
#include "artdaq-database/ConfigurationDB/dboperation_mongodb.h"

#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/FhiclJson/shared_literals.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/JsonDocument/JSONDocument_template.h"

#include <boost/exception/diagnostic_information.hpp>

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "CONF:OpLdStr_C"

namespace db = artdaq::database;
namespace dbjsu = db::jsonutils;
namespace dbjsul = db::jsonutils::literal;
namespace cf = db::configuration;
namespace cfgui = cf::guiexports;
namespace cfo = cf::options;
namespace cfol = cfo::literal;

using cfo::LoadStoreOperation;
using cfo::data_format_t;

namespace jul = artdaq::database::jsonutils::literal;
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
void store_configuration(LoadStoreOperation const&, std::string&);
void load_configuration(LoadStoreOperation const&, std::string&);
}

void LoadStoreOperation::read(std::string const& search_filter) {
  assert(!search_filter.empty());

  JsonSerializable::read(search_filter);

  auto filter_document = JSONDocument{search_filter};

  try {
    _version = filter_document.value_as<std::string>(cfol::version);
  } catch (...) {
  }

  try {
    _configurable_entity = filter_document.value_as<std::string>(cfol::configurable_entity);
  } catch (...) {
  }

  try {
    type(filter_document.value_as<std::string>(cfol::collection));
  } catch (...) {
  }
}

std::string const& LoadStoreOperation::type() const noexcept {
  assert(!_type.empty());

  return _type;
}

std::string const& LoadStoreOperation::type(std::string const& type) {
  assert(!type.empty());

  if (type.empty()) {
    throw cet::exception("Options") << "Invalid type; type is empty.";
  }

  TRACE_(12, "Options: Updating type from " << _type << " to " << type << ".");

  _type = type;

  return _type;
}

std::string const& LoadStoreOperation::version() const noexcept {
  assert(!_version.empty());

  return _version;
}

std::string const& LoadStoreOperation::version(std::string const& version) {
  assert(!version.empty());

  if (version.empty()) {
    throw cet::exception("Options") << "Invalid version; version is empty.";
  }

  TRACE_(13, "Options: Updating version from " << _version << " to " << version << ".");

  _version = version;

  return _version;
}

std::string const& LoadStoreOperation::configurableEntity() const noexcept {
  assert(!_configurable_entity.empty());

  return _configurable_entity;
}

std::string const& LoadStoreOperation::configurableEntity(std::string const& entity) {
  assert(!entity.empty());

  if (entity.empty()) {
    throw cet::exception("Options") << "Invalid version; entity is empty.";
  }

  TRACE_(13, "Options: Updating entity from " << _configurable_entity << " to " << entity << ".");

  _configurable_entity = entity;

  return _configurable_entity;
}

JSONDocument LoadStoreOperation::version_jsndoc() const {
  auto kvp = std::make_pair<std::string, std::string>(jul::version, _version.c_str());
  return toJSONDocument(kvp);
}

JSONDocument LoadStoreOperation::configurableEntity_jsndoc() const {
  auto kvp = std::make_pair<std::string, std::string>(jul::name, _configurable_entity.c_str());

  return toJSONDocument(kvp);
}

JSONDocument LoadStoreOperation::search_filter_jsondoc() const {
  if (searchFilter() != cfol::notprovided) {
    return {searchFilter()};
  }

  std::stringstream ss;
  ss << "{";

  if (globalConfigurationId() != cfol::notprovided) {
    ss << cf::quoted_(jul::configurations_name) << cfol::colon << cf::quoted_(globalConfigurationId());
  }

  if (_version != cfol::notprovided) {
    ss << cf::quoted_(jul::version) << cfol::colon << cf::quoted_(_version);
  }

  if (_configurable_entity != cfol::notprovided) {
    ss << cf::quoted_(jul::configurable_entity_name) << cfol::colon << cf::quoted_(_configurable_entity);
  }

  ss << "}";

  return {ss.str()};
}

cf::result_pair_t cf::store_configuration(LoadStoreOperation const& options, std::string& conf) noexcept {
  try {
    detail::store_configuration(options, conf);
    return cf::result_pair_t{true, "Success"};
  } catch (...) {
    return cf::result_pair_t{false, boost::current_exception_diagnostic_information()};
  }
}

cf::result_pair_t cf::load_configuration(LoadStoreOperation const& options, std::string& conf) noexcept {
  try {
    detail::load_configuration(options, conf);
    return cf::result_pair_t{true, "Success"};
  } catch (...) {
    return cf::result_pair_t{false, boost::current_exception_diagnostic_information()};
  }
}

cf::result_pair_t cfgui::store_configuration(std::string const& search_filter, std::string const& conf) noexcept {
  try {
    auto options = LoadStoreOperation{};
    options.read(search_filter);

    // convert to database_format
    auto database_format = std::string(conf);

    detail::store_configuration(options, database_format);
    return cf::result_pair_t{true, database_format};
  } catch (...) {
    return cf::result_pair_t{false, boost::current_exception_diagnostic_information()};
  }
}

cf::result_pair_t cfgui::load_configuration(std::string const& search_filter, std::string& conf) noexcept {
  try {
    auto options = LoadStoreOperation{};
    options.read(search_filter);

    auto database_format = std::string(conf);

    detail::load_configuration(options, database_format);

    // convert to gui
    conf = database_format;

    return cf::result_pair_t{true, "Success"};
  } catch (...) {
    return cf::result_pair_t{false, boost::current_exception_diagnostic_information()};
  }
}

void cf::trace_enable_LoadStoreOperation() {
  trace_enable_LoadStoreOperationDetail();

  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", 1LL);
  TRACE_CNTL("modeS", 1LL);

  TRACE_(0, "artdaq::database::configuration::LoadStoreOperation"
                << "trace_enable");
}
