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
namespace apiliteral = artdaq::database::configapi::literal;

ManageAliasesOperation::ManageAliasesOperation(std::string const& process_name) : OperationBase{process_name} {}

std::string const& ManageAliasesOperation::version() const {
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

std::string const& ManageAliasesOperation::entity() const {
  confirm(!_entity.empty());

  return _entity;
}

std::string const& ManageAliasesOperation::entity(std::string const& entity) {
  confirm(!entity.empty());

  if (entity.empty()) {
    throw runtime_error("Options") << "Invalid configurable entity; entity is empty.";
  }

  TRACE_(11, "Options: Updating entity from " << _entity << " to " << entity << ".");

  _entity = entity;

  return _entity;
}

std::string const& ManageAliasesOperation::run() const {
  confirm(!_run.empty());

  return _run;
}

std::string const& ManageAliasesOperation::run(std::string const& run) {
  confirm(!run.empty());

  if (run.empty()) {
    throw runtime_error("Options") << "Invalid run; run is empty.";
  }

  TRACE_(10, "Options: Updating run from " << _run << " to " << run << ".");

  _run = run;

  return _run;
}

std::string const& ManageAliasesOperation::configuration() const {
  confirm(!_configuration.empty());

  return _configuration;
}

std::string const& ManageAliasesOperation::configuration(std::string const& global_configuration) {
  confirm(!global_configuration.empty());

  if (global_configuration.empty()) {
    throw runtime_error("Options") << "Invalid global config; global config is empty.";
  }

  TRACE_(12, "Options: Updating global_configuration from " << _configuration << " to " << global_configuration << ".");

  _configuration = global_configuration;

  return _configuration;
}

std::string const& ManageAliasesOperation::versionAlias() const {
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

std::string const& ManageAliasesOperation::configurationAlias() const {
  confirm(!_configuration_alias.empty());

  return _configuration_alias;
}

std::string const& ManageAliasesOperation::configurationAlias(std::string const& global_configuration_alias) {
  confirm(!global_configuration_alias.empty());

  if (global_configuration_alias.empty()) {
    throw runtime_error("Options") << "Invalid global configuration alias; global configuration alias is empty.";
  }

  TRACE_(10, "Options: Updating global configuration alias from " << _configuration_alias << " to "
                                                                  << global_configuration_alias << ".");

  _configuration_alias = global_configuration_alias;

  return _configuration_alias;
}

void ManageAliasesOperation::readJsonData(JsonData const& data) {
  confirm(!data.empty());

  OperationBase::readJsonData(data);

  using namespace artdaq::database::json;
  auto dataAST = object_t{};

  if (!JsonReader{}.read(data, dataAST)) {
    throw db::invalid_option_exception("ManageAliasesOperation")
        << "ManageAliasesOperation: Unable to read JSON buffer.";
  }

  try {
    auto const& filterAST = boost::get<jsn::object_t>(dataAST.at(apiliteral::option::searchfilter));

    if (filterAST.empty()) queryFilter(apiliteral::notprovided);

    try {
      entity(boost::get<std::string>(filterAST.at(apiliteral::filter::entities)));
    } catch (...) {
      try {
        entity(boost::get<std::string>(dataAST.at(apiliteral::option::entity)));
      } catch (...) {
      }
    }

    try {
      versionAlias(boost::get<std::string>(filterAST.at(apiliteral::filter::version_aliases)));
    } catch (...) {
      try {
        versionAlias(boost::get<std::string>(dataAST.at(apiliteral::option::version_alias)));
      } catch (...) {
      }
    }

    try {
      configurationAlias(boost::get<std::string>(filterAST.at(apiliteral::filter::configuration_aliases)));
    } catch (...) {
      try {
        configurationAlias(boost::get<std::string>(dataAST.at(apiliteral::option::configuration_alias)));
      } catch (...) {
      }
    }

    try {
      version(boost::get<std::string>(filterAST.at(apiliteral::filter::version)));
    } catch (...) {
      try {
        version(boost::get<std::string>(dataAST.at(apiliteral::option::version)));
      } catch (...) {
      }
    }

    try {
      configuration(boost::get<std::string>(filterAST.at(apiliteral::filter::configurations)));
    } catch (...) {
      try {
        configuration(boost::get<std::string>(dataAST.at(apiliteral::option::configuration)));
      } catch (...) {
      }
    }

    try {
      run(boost::get<std::string>(filterAST.at(apiliteral::filter::run)));
    } catch (...) {
      try {
        run(boost::get<std::string>(dataAST.at(apiliteral::option::run)));
      } catch (...) {
      }
    }

  } catch (...) {
    TRACE_(1, "Options() no filter provided <" << data << ">");
  }
}

int ManageAliasesOperation::readProgramOptions(bpo::variables_map const& vm) {
  auto result = OperationBase::readProgramOptions(vm);

  if (result != process_exit_code::SUCCESS) return result;

  if (vm.count(apiliteral::option::entity)) {
    entity(vm[apiliteral::option::entity].as<std::string>());
  }

  if (vm.count(apiliteral::option::configuration)) {
    configuration(vm[apiliteral::option::configuration].as<std::string>());
  }

  if (vm.count(apiliteral::option::version)) {
    version(vm[apiliteral::option::version].as<std::string>());
  }

  if (vm.count(apiliteral::option::run)) {
    run(vm[apiliteral::option::run].as<std::string>());
  }

  if (vm.count(apiliteral::option::version_alias)) {
    versionAlias(vm[apiliteral::option::version_alias].as<std::string>());
  }

  if (vm.count(apiliteral::option::configuration_alias)) {
    configurationAlias(vm[apiliteral::option::configuration_alias].as<std::string>());
  }

  return process_exit_code::SUCCESS;
}

bpo::options_description ManageAliasesOperation::makeProgramOptions() const {
  auto opts = OperationBase::makeProgramOptions();

  auto make_opt_name = [](auto& long_name, auto& short_name) {
    return std::string{long_name}.append(",").append(short_name);
  };

  opts.add_options()(make_opt_name(apiliteral::option::version, "v").c_str(), bpo::value<std::string>(), "Version");

  opts.add_options()(make_opt_name(apiliteral::option::version_alias, "a").c_str(), bpo::value<std::string>(),
                     "Version Alias");

  opts.add_options()(make_opt_name(apiliteral::option::entity, "e").c_str(), bpo::value<std::string>(),
                     "Configurable-entity name");
  opts.add_options()(make_opt_name(apiliteral::option::configuration, "g").c_str(), bpo::value<std::string>(),
                     "Configuration name");

  opts.add_options()(make_opt_name(apiliteral::option::configuration_alias, "q").c_str(), bpo::value<std::string>(),
                     "Configuration Alias");

  opts.add_options()(make_opt_name(apiliteral::option::run, "r").c_str(), bpo::value<std::string>(), "Run");

  return opts;
}

JsonData ManageAliasesOperation::writeJsonData() const {
  using namespace artdaq::database::json;

  auto docAST = object_t{};

  if (!JsonReader{}.read(OperationBase::writeJsonData(), docAST)) {
    throw db::invalid_option_exception("ManageAliasesOperation") << "Unable to read query_filter_to_JsonData().";
  }

  if (configuration() != apiliteral::notprovided) docAST[apiliteral::option::configuration] = configuration();
  if (versionAlias() != apiliteral::notprovided) docAST[apiliteral::option::version_alias] = versionAlias();
  if (configurationAlias() != apiliteral::notprovided)
    docAST[apiliteral::option::configuration_alias] = configurationAlias();
  if (version() != apiliteral::notprovided) docAST[apiliteral::option::version] = version();

  if (run() != apiliteral::notprovided) docAST[apiliteral::option::run] = run();

  auto json_buffer = std::string{};

  if (!JsonWriter{}.write(docAST, json_buffer)) {
    throw db::invalid_option_exception("ManageAliasesOperation") << "Unable to write JSON buffer.";
  }
  return {json_buffer};
}

JsonData ManageAliasesOperation::query_filter_to_JsonData() const {
  if (queryFilter() != apiliteral::notprovided) {
    return {queryFilter()};
  }
  using namespace artdaq::database::json;

  auto docAST = object_t{};

  if (!JsonReader{}.read(OperationBase::query_filter_to_JsonData(), docAST)) {
    throw db::invalid_option_exception("ManageAliasesOperation") << "Unable to query_filter_to_JsonData().";
  }

  if (entity() != apiliteral::notprovided) docAST[apiliteral::filter::entities] = entity();

  if (version() != apiliteral::notprovided) docAST[apiliteral::filter::version] = version();

  if (configuration() != apiliteral::notprovided && operation() != apiliteral::operation::assignconfig)
    docAST[apiliteral::filter::configurations] = configuration();

  if (versionAlias() != apiliteral::notprovided && operation() != apiliteral::operation::addversionalias)
    docAST[apiliteral::filter::version_aliases] = versionAlias();

  if (run() != apiliteral::notprovided && operation() != apiliteral::operation::addrun)
    docAST[apiliteral::filter::runs] = run();

  if (docAST.empty()) {
    return {apiliteral::empty_json};
  }

  auto json_buffer = std::string{};

  if (!JsonWriter{}.write(docAST, json_buffer)) {
    throw db::invalid_option_exception("ManageAliasesOperation") << "Unable to write JSON buffer.";
  }
  return {json_buffer};
}

JsonData ManageAliasesOperation::versionAlias_to_JsonData() const {
  using namespace artdaq::database::json;
  auto docAST = object_t{};

  docAST[apiliteral::name] = versionAlias();

  auto json_buffer = std::string{};

  if (!JsonWriter{}.write(docAST, json_buffer)) {
    throw db::invalid_option_exception("ManageConfigsOperation") << "Unable to write JSON buffer.";
  }

  return {json_buffer};
}

JsonData ManageAliasesOperation::run_to_JsonData() const {
  using namespace artdaq::database::json;
  auto docAST = object_t{};

  docAST[apiliteral::name] = run();

  auto json_buffer = std::string{};

  if (!JsonWriter{}.write(docAST, json_buffer)) {
    throw db::invalid_option_exception("ManageAliasesOperation") << "Unable to write JSON buffer.";
  }

  return {json_buffer};
}

//
void cf::debug::options::ManageAliases() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);

  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TRACE_(0, "artdaq::database::configuration::options::ManageAliases trace_enable");
}
