#include "artdaq-database/BuildInfo/process_exceptions.h"
#include "artdaq-database/ConfigurationDB/common.h"

#include "artdaq-database/ConfigurationDB/options_operation_base.h"
#include "artdaq-database/ConfigurationDB/options_operation_loadstore.h"

#include "artdaq-database/ConfigurationDB/shared_helper_functions.h"

#include "artdaq-database/FhiclJson/shared_literals.h"

#include "artdaq-database/BasicTypes/basictypes.h"

#include "artdaq-database/FhiclJson/json_reader.h"
#include "artdaq-database/FhiclJson/json_writer.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "CONF:OpLdStr_C"

namespace db = artdaq::database;
namespace cf = db::configuration;
namespace cfl = cf::literal;
namespace dbbt = db::basictypes;

using dbbt::JsonData;
using cf::OperationBase;
using cf::LoadStoreOperation;

using cf::options::data_format_t;

LoadStoreOperation::LoadStoreOperation(std::string const& process_name) : OperationBase{process_name} {}

std::string const& LoadStoreOperation::version() const noexcept {
  assert(!_version.empty());

  return _version;
}

std::string const& LoadStoreOperation::version(std::string const& version) {
  assert(!version.empty());

  if (version.empty()) {
    throw cet::exception("Options") << "Invalid version; version is empty.";
  }

  TRACE_(10, "Options: Updating version from " << _version << " to " << version << ".");

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
    throw cet::exception("Options") << "Invalid configurable entity; entity is empty.";
  }

  TRACE_(11, "Options: Updating entity from " << _configurable_entity << " to " << entity << ".");

  _configurable_entity = entity;

  return _configurable_entity;
}

std::string const& LoadStoreOperation::globalConfiguration() const noexcept {
  assert(!_global_configuration.empty());

  return _global_configuration;
}

std::string const& LoadStoreOperation::globalConfiguration(std::string const& global_configuration) {
  assert(!global_configuration.empty());

  if (global_configuration.empty()) {
    throw cet::exception("Options") << "Invalid global config; global config is empty.";
  }

  TRACE_(12, "Options: Updating global_configuration from " << _global_configuration << " to " << global_configuration
                                                            << ".");

  _global_configuration = global_configuration;

  return _global_configuration;
}

void LoadStoreOperation::readJsonData(JsonData const& data) {
  assert(!data.json_buffer.empty());

  OperationBase::readJsonData(data);

  using namespace artdaq::database::json;
  auto filterAST = object_t{};

  if (!JsonReader{}.read(data.json_buffer, filterAST)) {
    throw db::invalid_option_exception("Options") << "Unable to read JSON buffer.";
  }

  TRACE_(1, "Options() read filterAST");

  try {
    version(boost::get<std::string>(filterAST.at(cfl::option::version)));
  } catch (...) {
  }

  try {
    configurableEntity(boost::get<std::string>(filterAST.at(cfl::option::entity)));
  } catch (...) {
  }

  try {
    globalConfiguration(boost::get<std::string>(filterAST.at(cfl::option::configuration)));
  } catch (...) {
  }
}

int LoadStoreOperation::readProgramOptions(bpo::variables_map const& vm) {
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

bpo::options_description LoadStoreOperation::makeProgramOptions() const {
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

JsonData LoadStoreOperation::writeJsonData() const {
  using namespace artdaq::database::json;

  auto docAST = object_t{};

  if (!JsonReader{}.read(OperationBase::writeJsonData().json_buffer, docAST)) {
    throw db::invalid_option_exception("Options") << "Unable to readsearch_filter_to_JsonData().";
  }

  if (globalConfiguration() != cfl::notprovided) docAST[cfl::option::configuration] = globalConfiguration();

  auto json_buffer = std::string{};

  if (!JsonWriter{}.write(docAST, json_buffer)) {
    throw db::invalid_option_exception("Options") << "Unable to write JSON buffer.";
  }
  return {json_buffer};
}

JsonData LoadStoreOperation::search_filter_to_JsonData() const {
  if (searchFilter() != cfl::notprovided) {
    return {searchFilter()};
  }

  using namespace artdaq::database::json;

  auto docAST = object_t{};

  if (!JsonReader{}.read(OperationBase::search_filter_to_JsonData().json_buffer, docAST)) {
    throw db::invalid_option_exception("Options") << "Unable to search_filter_to_JsonData().";
  }

  if (version() != cfl::notprovided) docAST[cfl::filter::version] = version();

  if (configurableEntity() != cfl::notprovided) docAST[cfl::filter::entity] = configurableEntity();

  if (globalConfiguration() != cfl::notprovided && operation() != cfl::operation::addconfig)
    docAST[cfl::filter::configuration] = globalConfiguration();

  auto json_buffer = std::string{};

  if (!JsonWriter{}.write(docAST, json_buffer)) {
    throw db::invalid_option_exception("Options") << "Unable to write JSON buffer.";
  }
  return {json_buffer};
}

//
void cf::trace_enable_OperationLoadStore() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);

  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TRACE_(0, "artdaq::database::configuration::OperationLoadStore"
                << "trace_enable");
}
