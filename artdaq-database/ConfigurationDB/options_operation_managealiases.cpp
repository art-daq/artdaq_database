#include "artdaq-database/ConfigurationDB/common.h"

#include "artdaq-database/ConfigurationDB/options_operation_base.h"
#include "artdaq-database/ConfigurationDB/options_operation_managealiases.h"

#include "artdaq-database/ConfigurationDB/shared_helper_functions.h"

#include "artdaq-database/DataFormats/shared_literals.h"

#include "artdaq-database/BasicTypes/basictypes.h"

#include "artdaq-database/DataFormats/Json/json_reader.h"
#include "artdaq-database/DataFormats/Json/json_writer.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "CONF:OpMgAl_C"

namespace db = artdaq::database;
namespace cf = db::configuration;
namespace dbbt = db::basictypes;

using dbbt::JsonData;
using cf::OperationBase;
using cf::ManageAliasesOperation;

using cf::options::data_format_t;

ManageAliasesOperation::ManageAliasesOperation(std::string const& process_name) : OperationBase{process_name} {}

std::string const& ManageAliasesOperation::version() const noexcept {
  confirm(!_version.empty());

  return _version;
}

std::string const& ManageAliasesOperation::version(std::string const& version) {
  confirm(!version.empty());

  if (version.empty()) {
    throw runtime_error("Options") << "Invalid version; version is empty.";
  }

  TRACE_(10, "Options: Updating version from " << _version << " to " << version << ".");

  _version = version;

  return _version;
}

std::string const& ManageAliasesOperation::configurableEntity() const noexcept {
  confirm(!_configurable_entity.empty());

  return _configurable_entity;
}

std::string const& ManageAliasesOperation::configurableEntity(std::string const& entity) {
  confirm(!entity.empty());

  if (entity.empty()) {
    throw runtime_error("Options") << "Invalid configurable entity; entity is empty.";
  }

  TRACE_(11, "Options: Updating entity from " << _configurable_entity << " to " << entity << ".");

  _configurable_entity = entity;

  return _configurable_entity;
}

std::string const& ManageAliasesOperation::globalConfiguration() const noexcept {
  confirm(!_global_configuration.empty());

  return _global_configuration;
}

std::string const& ManageAliasesOperation::globalConfiguration(std::string const& global_configuration) {
  confirm(!global_configuration.empty());

  if (global_configuration.empty()) {
    throw runtime_error("Options") << "Invalid global config; global config is empty.";
  }

  TRACE_(12, "Options: Updating global_configuration from " << _global_configuration << " to " << global_configuration
                                                            << ".");

  _global_configuration = global_configuration;

  return _global_configuration;
}

std::string const& ManageAliasesOperation::versionAlias() const noexcept {
  confirm(!_version_alias.empty());

  return _version_alias;
}

std::string const& ManageAliasesOperation::versionAlias(std::string const& alias) {
  confirm(!alias.empty());

  if (alias.empty()) {
    throw runtime_error("Options") << "Invalid version alias; version alias is empty.";
  }

  TRACE_(10, "Options: Updating version alias from " << _version_alias << " to " << alias << ".");

  _version_alias = alias;

  return _version_alias;
}

std::string const& ManageAliasesOperation::globalConfigurationAlias() const noexcept {
  confirm(!_global_configuration_alias.empty());

  return _global_configuration_alias;
}

std::string const& ManageAliasesOperation::globalConfigurationAlias(std::string const& global_configuration_alias) {
  confirm(!global_configuration_alias.empty());

  if (global_configuration_alias.empty()) {
    throw runtime_error("Options") << "Invalid global configuration alias; "
                                      "global configuration alias is empty.";
  }

  TRACE_(10, "Options: Updating global configuration alias from " << _global_configuration_alias << " to "
                                                                  << global_configuration_alias << ".");

  _global_configuration_alias = global_configuration_alias;

  return _global_configuration_alias;
}

void ManageAliasesOperation::readJsonData(JsonData const& data) {
  confirm(!data.json_buffer.empty());

  OperationBase::readJsonData(data);

  using namespace artdaq::database::json;
  auto dataAST = object_t{};

  if (!JsonReader{}.read(data.json_buffer, dataAST)) {
    throw db::invalid_option_exception("ManageAliasesOperation")
        << "ManageAliasesOperation: Unable to read JSON buffer.";
  }

  try {
    auto const& filterAST = boost::get<jsn::object_t>(dataAST.at(apiliteral::option::searchfilter));

    if (filterAST.empty()) searchFilter(jsonliteral::notprovided);

    try {
      configurableEntity(boost::get<std::string>(filterAST.at(apiliteral::option::entity)));
    } catch (...) {
    }

    try {
      versionAlias(boost::get<std::string>(filterAST.at(apiliteral::option::version_alias)));
    } catch (...) {
    }

    try {
      globalConfigurationAlias(boost::get<std::string>(filterAST.at(apiliteral::option::configuration_alias)));
    } catch (...) {
    }

    try {
      version(boost::get<std::string>(filterAST.at(apiliteral::option::version)));
    } catch (...) {
    }

    try {
      globalConfiguration(boost::get<std::string>(filterAST.at(apiliteral::option::configuration)));
    } catch (...) {
    }

  } catch (...) {
    TRACE_(1, "Options() no filter provided <" << data << ">");
  }
}

int ManageAliasesOperation::readProgramOptions(bpo::variables_map const& vm) {
  auto result = OperationBase::readProgramOptions(vm);

  if (result != process_exit_code::SUCCESS) return result;

  if (vm.count(apiliteral::option::entity)) {
    configurableEntity(vm[apiliteral::option::entity].as<std::string>());
  }

  if (vm.count(apiliteral::option::configuration)) {
    globalConfiguration(vm[apiliteral::option::configuration].as<std::string>());
  }

  if (vm.count(apiliteral::option::version)) {
    version(vm[apiliteral::option::version].as<std::string>());
  }

  if (vm.count(apiliteral::option::version_alias)) {
    versionAlias(vm[apiliteral::option::version_alias].as<std::string>());
  }

  if (vm.count(apiliteral::option::configuration_alias)) {
    globalConfigurationAlias(vm[apiliteral::option::configuration_alias].as<std::string>());
  }

  return process_exit_code::SUCCESS;
}

bpo::options_description ManageAliasesOperation::makeProgramOptions() const {
  auto opts = OperationBase::makeProgramOptions();

  auto make_opt_name = [](auto& long_name, auto& short_name) {
    return std::string{long_name}.append(",").append(short_name);
  };

  opts.add_options()(make_opt_name(apiliteral::option::version, "v").c_str(), bpo::value<std::string>(),
                     "Configuration version");

  opts.add_options()(make_opt_name(apiliteral::option::version_alias, "a").c_str(), bpo::value<std::string>(),
                     "Version alias");

  opts.add_options()(make_opt_name(apiliteral::option::entity, "e").c_str(), bpo::value<std::string>(),
                     "Configurable-entity name");
  opts.add_options()(make_opt_name(apiliteral::option::configuration, "g").c_str(), bpo::value<std::string>(),
                     "Global configuration name");

  opts.add_options()(make_opt_name(apiliteral::option::configuration_alias, "q").c_str(), bpo::value<std::string>(),
                     "Configuration alias");

  return opts;
}

JsonData ManageAliasesOperation::writeJsonData() const {
  using namespace artdaq::database::json;

  auto docAST = object_t{};

  if (!JsonReader{}.read(OperationBase::writeJsonData().json_buffer, docAST)) {
    throw db::invalid_option_exception("ManageAliasesOperation") << "Unable to readsearch_filter_to_JsonData().";
  }

  if (globalConfiguration() != jsonliteral::notprovided)
    docAST[apiliteral::option::configuration] = globalConfiguration();
  if (versionAlias() != jsonliteral::notprovided) docAST[apiliteral::option::version_alias] = versionAlias();
  if (globalConfigurationAlias() != jsonliteral::notprovided)
    docAST[apiliteral::option::configuration_alias] = globalConfigurationAlias();
  if (version() != jsonliteral::notprovided) docAST[apiliteral::option::version] = version();

  auto json_buffer = std::string{};

  if (!JsonWriter{}.write(docAST, json_buffer)) {
    throw db::invalid_option_exception("ManageAliasesOperation") << "Unable to write JSON buffer.";
  }
  return {json_buffer};
}

JsonData ManageAliasesOperation::search_filter_to_JsonData() const {
  if (searchFilter() != jsonliteral::notprovided) {
    return {searchFilter()};
  }

  using namespace artdaq::database::json;

  auto docAST = object_t{};

  if (!JsonReader{}.read(OperationBase::search_filter_to_JsonData().json_buffer, docAST)) {
    throw db::invalid_option_exception("ManageAliasesOperation") << "Unable to search_filter_to_JsonData().";
  }

  if (configurableEntity() != jsonliteral::notprovided) docAST[apiliteral::filter::entity] = configurableEntity();

  if (globalConfiguration() != jsonliteral::notprovided && operation() != apiliteral::operation::addconfig)
    docAST[apiliteral::filter::configuration] = globalConfiguration();

  if (docAST.empty()) {
    return {apiliteral::empty_json};
  }

  auto json_buffer = std::string{};

  if (!JsonWriter{}.write(docAST, json_buffer)) {
    throw db::invalid_option_exception("ManageAliasesOperation") << "Unable to write JSON buffer.";
  }
  return {json_buffer};
}

//
void cf::debug::options::enableOperationManageAliases() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);

  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TRACE_(0,
         "artdaq::database::configuration::options::OperationManageAliases "
         "trace_enable");
}
