#include "artdaq-database/ConfigurationDB/common.h"

#include "artdaq-database/ConfigurationDB/options_operation_base.h"
#include "artdaq-database/ConfigurationDB/options_operation_loadstore.h"

#include "artdaq-database/ConfigurationDB/shared_helper_functions.h"

#include "artdaq-database/DataFormats/shared_literals.h"

#include "artdaq-database/BasicTypes/basictypes.h"

#include "artdaq-database/DataFormats/Json/json_reader.h"
#include "artdaq-database/DataFormats/Json/json_writer.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "CONF:OpLdStr_C"

namespace db = artdaq::database;
namespace cf = db::configuration;
namespace dbbt = db::basictypes;

using dbbt::JsonData;
using cf::OperationBase;
using cf::LoadStoreOperation;

using cf::options::data_format_t;

LoadStoreOperation::LoadStoreOperation(std::string const& process_name) : OperationBase{process_name} {}

std::string const& LoadStoreOperation::version() const noexcept {
  confirm(!_version.empty());

  return _version;
}

std::string const& LoadStoreOperation::version(std::string const& version) {
  confirm(!version.empty());

  if (version.empty()) {
    throw runtime_error("Options") << "Invalid version; version is empty.";
  }

  TRACE_(10, "Options: Updating version from " << _version << " to " << version << ".");

  _version = version;

  return _version;
}

std::string const& LoadStoreOperation::configurableEntity() const noexcept {
  confirm(!_configurable_entity.empty());

  return _configurable_entity;
}

std::string const& LoadStoreOperation::configurableEntity(std::string const& entity) {
  confirm(!entity.empty());

  if (entity.empty()) {
    throw runtime_error("Options") << "Invalid configurable entity; entity is empty.";
  }

  TRACE_(11, "Options: Updating entity from " << _configurable_entity << " to " << entity << ".");

  _configurable_entity = entity;

  return _configurable_entity;
}

std::string const& LoadStoreOperation::globalConfiguration() const noexcept {
  confirm(!_global_configuration.empty());

  return _global_configuration;
}

std::string const& LoadStoreOperation::globalConfiguration(std::string const& global_configuration) {
  confirm(!global_configuration.empty());

  if (global_configuration.empty()) {
    throw runtime_error("Options") << "Invalid global config; global config is empty.";
  }

  TRACE_(12, "Options: Updating global_configuration from " << _global_configuration << " to " << global_configuration
                                                            << ".");

  _global_configuration = global_configuration;

  return _global_configuration;
}

std::string const& LoadStoreOperation::sourceFileName() const noexcept {
  confirm(!_source_file_name.empty());

  return _source_file_name;
}

std::string const& LoadStoreOperation::sourceFileName(std::string const& source_file_name) {
  confirm(!source_file_name.empty());

  if (source_file_name.empty()) {
    throw runtime_error("Options") << "Invalid source file name; source file name is empty.";
  }

  TRACE_(12, "Options: Updating source_file_name from " << _source_file_name << " to " << source_file_name << ".");

  _source_file_name = source_file_name;

  return _source_file_name;
}

void LoadStoreOperation::readJsonData(JsonData const& data) {
  confirm(!data.json_buffer.empty());

  OperationBase::readJsonData(data);

  using namespace artdaq::database::json;
  auto dataAST = object_t{};

  if (!JsonReader{}.read(data.json_buffer, dataAST)) {
    throw db::invalid_option_exception("LoadStoreOperation") << "LoadStoreOperation: Unable to read JSON buffer.";
  }

  try {
    globalConfiguration(boost::get<std::string>(dataAST.at(apiliteral::option::configuration)));
  } catch (...) {
  }

  try {
    version(boost::get<std::string>(dataAST.at(apiliteral::option::version)));
  } catch (...) {
  }

  try {
    configurableEntity(boost::get<std::string>(dataAST.at(apiliteral::gui::configurable_entity)));
  } catch (...) {
  }

  try {
    sourceFileName(boost::get<std::string>(dataAST.at(apiliteral::option::source)));
  } catch (...) {
  }

  try {
    auto const& filterAST = boost::get<jsn::object_t>(dataAST.at(apiliteral::option::searchfilter));

    if (filterAST.empty()) searchFilter(jsonliteral::notprovided);

    try {
      version(boost::get<std::string>(filterAST.at(apiliteral::filter::version)));
    } catch (...) {
    }

    try {
      configurableEntity(boost::get<std::string>(filterAST.at(apiliteral::filter::entity)));
    } catch (...) {
    }

    try {
      globalConfiguration(boost::get<std::string>(filterAST.at(apiliteral::filter::configuration)));
    } catch (...) {
    }

    try {
      sourceFileName(boost::get<std::string>(dataAST.at(apiliteral::option::source)));
    } catch (...) {
    }

  } catch (...) {
    TRACE_(1, "Options() no filter provided <" << data << ">");
  }
}

int LoadStoreOperation::readProgramOptions(bpo::variables_map const& vm) {
  auto result = OperationBase::readProgramOptions(vm);

  if (result != process_exit_code::SUCCESS) return result;

  if (vm.count(apiliteral::option::version)) {
    version(vm[apiliteral::option::version].as<std::string>());
  }

  if (vm.count(apiliteral::option::entity)) {
    configurableEntity(vm[apiliteral::option::entity].as<std::string>());
  }

  if (vm.count(apiliteral::option::configuration)) {
    globalConfiguration(vm[apiliteral::option::configuration].as<std::string>());
  }

  if (vm.count(apiliteral::option::source)) {
    sourceFileName(vm[apiliteral::option::source].as<std::string>());
  }

  return process_exit_code::SUCCESS;
}

bpo::options_description LoadStoreOperation::makeProgramOptions() const {
  auto opts = OperationBase::makeProgramOptions();

  auto make_opt_name = [](auto& long_name, auto& short_name) {
    return std::string{long_name}.append(",").append(short_name);
  };

  opts.add_options()(make_opt_name(apiliteral::option::version, "v").c_str(), bpo::value<std::string>(),
                     "Configuration version");
  opts.add_options()(make_opt_name(apiliteral::option::entity, "e").c_str(), bpo::value<std::string>(),
                     "Configurable-entity name");
  opts.add_options()(make_opt_name(apiliteral::option::configuration, "g").c_str(), bpo::value<std::string>(),
                     "Global configuration name");

  opts.add_options()(make_opt_name(apiliteral::option::source, "s").c_str(), bpo::value<std::string>(),
                     "Configuration source file name");

  return opts;
}

JsonData LoadStoreOperation::writeJsonData() const {
  using namespace artdaq::database::json;

  auto docAST = object_t{};

  if (!JsonReader{}.read(OperationBase::writeJsonData().json_buffer, docAST)) {
    throw db::invalid_option_exception("LoadStoreOperation") << "Unable to readsearch_filter_to_JsonData().";
  }

  if (globalConfiguration() != jsonliteral::notprovided)
    docAST[apiliteral::option::configuration] = globalConfiguration();

  if (sourceFileName() != jsonliteral::notprovided) docAST[apiliteral::option::source] = sourceFileName();

  auto json_buffer = std::string{};

  if (!JsonWriter{}.write(docAST, json_buffer)) {
    throw db::invalid_option_exception("LoadStoreOperation") << "Unable to write JSON buffer.";
  }
  return {json_buffer};
}

JsonData LoadStoreOperation::search_filter_to_JsonData() const {
  if (searchFilter() != jsonliteral::notprovided) {
    return {searchFilter()};
  }

  using namespace artdaq::database::json;

  auto docAST = object_t{};

  if (!JsonReader{}.read(OperationBase::search_filter_to_JsonData().json_buffer, docAST)) {
    throw db::invalid_option_exception("LoadStoreOperation") << "Unable to search_filter_to_JsonData().";
  }

  if (version() != jsonliteral::notprovided) docAST[apiliteral::filter::version] = version();

  if (configurableEntity() != jsonliteral::notprovided) docAST[apiliteral::filter::entity] = configurableEntity();

  if (globalConfiguration() != jsonliteral::notprovided && operation() != apiliteral::operation::addconfig)
    docAST[apiliteral::filter::configuration] = globalConfiguration();

  if (docAST.empty()) {
    return {apiliteral::empty_json};
  }

  auto json_buffer = std::string{};

  if (!JsonWriter{}.write(docAST, json_buffer)) {
    throw db::invalid_option_exception("LoadStoreOperation") << "Unable to write JSON buffer.";
  }
  return {json_buffer};
}

JsonData LoadStoreOperation::globalConfiguration_to_JsonData() const {
  using namespace artdaq::database::json;
  auto docAST = object_t{};

  docAST[apiliteral::filter::configuration] = globalConfiguration();

  auto json_buffer = std::string{};

  if (!JsonWriter{}.write(docAST, json_buffer)) {
    throw db::invalid_option_exception("LoadStoreOperation") << "Unable to write JSON buffer.";
  }

  return {json_buffer};
}

JsonData LoadStoreOperation::collectionName_to_JsonData() const {
  using namespace artdaq::database::json;
  auto docAST = object_t{};

  docAST[apiliteral::option::collection] = collectionName();

  auto json_buffer = std::string{};

  if (!JsonWriter{}.write(docAST, json_buffer)) {
    throw db::invalid_option_exception("LoadStoreOperation") << "Unable to write JSON buffer.";
  }

  return {json_buffer};
}

JsonData LoadStoreOperation::version_to_JsonData() const {
  using namespace artdaq::database::json;
  auto docAST = object_t{};

  docAST[apiliteral::filter::version] = version();

  auto json_buffer = std::string{};

  if (!JsonWriter{}.write(docAST, json_buffer)) {
    throw db::invalid_option_exception("LoadStoreOperation") << "Unable to write JSON buffer.";
  }

  return {json_buffer};
}
JsonData LoadStoreOperation::configurableEntity_to_JsonData() const {
  using namespace artdaq::database::json;
  auto docAST = object_t{};

  docAST[apiliteral::name] = configurableEntity();

  auto json_buffer = std::string{};

  if (!JsonWriter{}.write(docAST, json_buffer)) {
    throw db::invalid_option_exception("LoadStoreOperation") << "Unable to write JSON buffer.";
  }

  return {json_buffer};
}

//
void cf::debug::options::enableLoadStoreOperation() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);

  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TRACE_(0,
         "artdaq::database::configuration::options::OperationLoadStore "
         "trace_enable");
}
