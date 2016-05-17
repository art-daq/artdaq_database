#include "artdaq-database/BuildInfo/process_exceptions.h"
#include "artdaq-database/ConfigurationDB/common.h"

#include "artdaq-database/ConfigurationDB/options_operation_base.h"
#include "artdaq-database/ConfigurationDB/options_operation_manageconfigs.h"

#include "artdaq-database/ConfigurationDB/shared_helper_functions.h"

#include "artdaq-database/DataFormats/common/shared_literals.h"

#include "artdaq-database/BasicTypes/basictypes.h"

#include "artdaq-database/DataFormats/Json/json_reader.h"
#include "artdaq-database/DataFormats/Json/json_writer.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "CONF:OpMgCf_C"

namespace db = artdaq::database;
namespace cf = db::configuration;
namespace cfl = cf::literal;
namespace dbbt = db::basictypes;

using dbbt::JsonData;
using cf::OperationBase;
using cf::ManageConfigsOperation;

using cf::options::data_format_t;

ManageConfigsOperation::ManageConfigsOperation(std::string const& process_name) : OperationBase{process_name} {}

std::string const& ManageConfigsOperation::version() const noexcept {
  assert(!_version.empty());

  return _version;
}

std::string const& ManageConfigsOperation::version(std::string const& version) {
  assert(!version.empty());

  if (version.empty()) {
    throw cet::exception("Options") << "Invalid version; version is empty.";
  }

  TRACE_(10, "Options: Updating version from " << _version << " to " << version << ".");

  _version = version;

  return _version;
}

std::string const& ManageConfigsOperation::configurableEntity() const noexcept {
  assert(!_configurable_entity.empty());

  return _configurable_entity;
}

std::string const& ManageConfigsOperation::configurableEntity(std::string const& entity) {
  assert(!entity.empty());

  if (entity.empty()) {
    throw cet::exception("Options") << "Invalid configurable entity; entity is empty.";
  }

  TRACE_(11, "Options: Updating entity from " << _configurable_entity << " to " << entity << ".");

  _configurable_entity = entity;

  return _configurable_entity;
}

std::string const& ManageConfigsOperation::globalConfiguration() const noexcept {
  assert(!_global_configuration.empty());

  return _global_configuration;
}

std::string const& ManageConfigsOperation::globalConfiguration(std::string const& global_configuration) {
  assert(!global_configuration.empty());

  if (global_configuration.empty()) {
    throw cet::exception("Options") << "Invalid global config; global config is empty.";
  }

  TRACE_(12, "Options: Updating global_configuration from " << _global_configuration << " to " << global_configuration
                                                            << ".");

  _global_configuration = global_configuration;

  return _global_configuration;
}

void ManageConfigsOperation::readJsonData(JsonData const& data) {
  assert(!data.json_buffer.empty());

  OperationBase::readJsonData(data);

  using namespace artdaq::database::json;
  auto dataAST = object_t{};

  if (!JsonReader{}.read(data.json_buffer, dataAST)) {
    throw db::invalid_option_exception("ManageConfigsOperation") << "Unable to read JSON buffer.";
  }

  try {
      globalConfiguration(boost::get<std::string>(dataAST.at(cfl::option::configuration)));
    } catch (...) {
  }
  
  try {
    auto const& filterAST = boost::get<jsn::object_t>(dataAST.at(cfl::option::searchfilter));

    if (filterAST.empty()) searchFilter(cfl::notprovided);

    try {
      configurableEntity(boost::get<std::string>(filterAST.at(cfl::filter::version)));
    } catch (...) {
    }
    
    try {
      configurableEntity(boost::get<std::string>(filterAST.at(cfl::filter::entity)));
    } catch (...) {
    }

    try {
      globalConfiguration(boost::get<std::string>(filterAST.at(cfl::filter::configuration)));
    } catch (...) {
    }

  } catch (...) {
    TRACE_(1, "Options() no filter provided <" << data << ">");
  }
}

int ManageConfigsOperation::readProgramOptions(bpo::variables_map const& vm) {
  auto result = OperationBase::readProgramOptions(vm);

  if (result != process_exit_code::SUCCESS) return result;

  if (vm.count(cfl::option::version)) {
    version(vm[cfl::option::version].as<std::string>());
  }

  if (vm.count(cfl::option::entity)) {
    configurableEntity(vm[cfl::option::entity].as<std::string>());
  }

  if (vm.count(cfl::option::configuration)) {
    globalConfiguration(vm[cfl::option::configuration].as<std::string>());
  }

  return process_exit_code::SUCCESS;
}

bpo::options_description ManageConfigsOperation::makeProgramOptions() const {
  auto opts = OperationBase::makeProgramOptions();

  auto make_opt_name = [](auto& long_name, auto& short_name) {
    return std::string{long_name}.append(",").append(short_name);
  };

  opts.add_options()(make_opt_name(cfl::option::version, "v").c_str(), bpo::value<std::string>(),
                     "Configuration version");
  opts.add_options()(make_opt_name(cfl::option::entity, "e").c_str(), bpo::value<std::string>(),
                     "Configurable-entity name");
  opts.add_options()(make_opt_name(cfl::option::configuration, "g").c_str(), bpo::value<std::string>(),
                     "Global configuration name");

  return opts;
}

JsonData ManageConfigsOperation::writeJsonData() const {
  using namespace artdaq::database::json;

  auto docAST = object_t{};

  if (!JsonReader{}.read(OperationBase::writeJsonData().json_buffer, docAST)) {
    throw db::invalid_option_exception("ManageConfigsOperation") << "Unable to readsearch_filter_to_JsonData().";
  }

  if (globalConfiguration() != cfl::notprovided) docAST[cfl::option::configuration] = globalConfiguration();

  auto json_buffer = std::string{};

  if (!JsonWriter{}.write(docAST, json_buffer)) {
    throw db::invalid_option_exception("ManageConfigsOperation") << "Unable to write JSON buffer.";
  }
  return {json_buffer};
}

JsonData ManageConfigsOperation::search_filter_to_JsonData() const {
  if (searchFilter() != cfl::notprovided) {
    return {searchFilter()};
  }

  using namespace artdaq::database::json;

  auto docAST = object_t{};

  if (!JsonReader{}.read(OperationBase::search_filter_to_JsonData().json_buffer, docAST)) {
    throw db::invalid_option_exception("ManageConfigsOperation") << "Unable to search_filter_to_JsonData().";
  }

  if (version() != cfl::notprovided) docAST[cfl::filter::version] = version();

  if (configurableEntity() != cfl::notprovided) docAST[cfl::filter::entity] = configurableEntity();

  if (globalConfiguration() != cfl::notprovided && operation() != cfl::operation::addconfig)
    docAST[cfl::filter::configuration] = globalConfiguration();

  if (docAST.empty()) {
    return {cfl::empty_json};
  }

  auto json_buffer = std::string{};

  if (!JsonWriter{}.write(docAST, json_buffer)) {
    throw db::invalid_option_exception("ManageConfigsOperation") << "Unable to write JSON buffer.";
  }
  return {json_buffer};
}


JsonData ManageConfigsOperation::globalConfiguration_to_JsonData() const {
  using namespace artdaq::database::json;
  auto docAST = object_t{};

  docAST[cfl::filter::configuration] = globalConfiguration();

  auto json_buffer = std::string{};

  if (!JsonWriter{}.write(docAST, json_buffer)) {
    throw db::invalid_option_exception("ManageConfigsOperation") << "Unable to write JSON buffer.";
  }

  return {json_buffer};
}

JsonData ManageConfigsOperation::version_to_JsonData() const {
  using namespace artdaq::database::json;
  auto docAST = object_t{};

  docAST[cfl::filter::version] = version();

  auto json_buffer = std::string{};

  if (!JsonWriter{}.write(docAST, json_buffer)) {
    throw db::invalid_option_exception("ManageConfigsOperation") << "Unable to write JSON buffer.";
  }

  return {json_buffer};
}

JsonData ManageConfigsOperation::configurableEntity_to_JsonData() const {
  using namespace artdaq::database::json;
  auto docAST = object_t{};

  docAST[cfl::name] = configurableEntity();

  auto json_buffer = std::string{};

  if (!JsonWriter{}.write(docAST, json_buffer)) {
    throw db::invalid_option_exception("ManageConfigsOperation") << "Unable to write JSON buffer.";
  }

  return {json_buffer};
}
//
void cf::debug::options::enableOperationManageConfigs() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);

  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TRACE_(0, "artdaq::database::configuration::options::OperationFindConfigs trace_enable");
}
