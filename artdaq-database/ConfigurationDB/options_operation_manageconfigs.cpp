#include "artdaq-database/ConfigurationDB/common.h"

#include "artdaq-database/ConfigurationDB/options_operation_base.h"
#include "artdaq-database/ConfigurationDB/options_operation_manageconfigs.h"

#include "artdaq-database/ConfigurationDB/shared_helper_functions.h"

#include "artdaq-database/DataFormats/shared_literals.h"

#include "artdaq-database/BasicTypes/basictypes.h"

#include "artdaq-database/DataFormats/Json/json_reader.h"
#include "artdaq-database/DataFormats/Json/json_writer.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "CONF:OpMgCf_C"

namespace db = artdaq::database;
namespace cf = db::configuration;
namespace dbbt = db::basictypes;

namespace jsonliteral = artdaq::database::dataformats::literal;
namespace apiliteral = artdaq::database::configapi::literal;

using dbbt::JsonData;
using cf::OperationBase;
using cf::ManageConfigsOperation;

using cf::options::data_format_t;

ManageConfigsOperation::ManageConfigsOperation(std::string const& process_name) : OperationBase{process_name} {}

std::string const& ManageConfigsOperation::version() const noexcept {
  confirm(!_version.empty());

  return _version;
}

std::string const& ManageConfigsOperation::version(std::string const& version) {
  confirm(!version.empty());

  if (version.empty()) {
    throw runtime_error("Options") << "Invalid version; version is empty.";
  }

  TRACE_(10, "Options: Updating version from " << _version << " to " << version << ".");

  _version = version;

  return _version;
}

std::string const& ManageConfigsOperation::entity() const noexcept {
  confirm(!_entity.empty());

  return _entity;
}

std::string const& ManageConfigsOperation::entity(std::string const& entity) {
  confirm(!entity.empty());

  if (entity.empty()) {
    throw runtime_error("Options") << "Invalid configurable entity; entity is empty.";
  }

  TRACE_(11, "Options: Updating entity from " << _entity << " to " << entity << ".");

  _entity = entity;

  return _entity;
}

std::string const& ManageConfigsOperation::configuration() const noexcept {
  confirm(!_configuration.empty());

  return _configuration;
}

std::string const& ManageConfigsOperation::configuration(std::string const& global_configuration) {
  confirm(!global_configuration.empty());

  if (global_configuration.empty()) {
    throw runtime_error("Options") << "Invalid global config; global config is empty.";
  }

  TRACE_(12, "Options: Updating global_configuration from " << _configuration << " to " << global_configuration << ".");

  _configuration = global_configuration;

  return _configuration;
}

void ManageConfigsOperation::readJsonData(JsonData const& data) {
  confirm(!data.json_buffer.empty());

  OperationBase::readJsonData(data);

  using namespace artdaq::database::json;
  auto dataAST = object_t{};

  if (!JsonReader{}.read(data.json_buffer, dataAST)) {
    throw db::invalid_option_exception("ManageConfigsOperation")
        << "ManageConfigsOperation: Unable to read JSON buffer.";
  }

  try {
    configuration(boost::get<std::string>(dataAST.at(apiliteral::option::configuration)));
  } catch (...) {
  }

  try {
    auto const& filterAST = boost::get<jsn::object_t>(dataAST.at(apiliteral::option::searchfilter));

    if (filterAST.empty()) queryFilter(jsonliteral::notprovided);

    try {
      version(boost::get<std::string>(filterAST.at(apiliteral::filter::version)));
    } catch (...) {
    }

    try {
      entity(boost::get<std::string>(filterAST.at(apiliteral::filter::entity)));
    } catch (...) {
    }

    try {
      configuration(boost::get<std::string>(filterAST.at(apiliteral::filter::configuration)));
    } catch (...) {
    }

  } catch (...) {
    TRACE_(1, "Options() no filter provided <" << data << ">");
  }
}

int ManageConfigsOperation::readProgramOptions(bpo::variables_map const& vm) {
  auto result = OperationBase::readProgramOptions(vm);

  if (result != process_exit_code::SUCCESS) return result;

  if (vm.count(apiliteral::option::version)) {
    version(vm[apiliteral::option::version].as<std::string>());
  }

  if (vm.count(apiliteral::option::entity)) {
    entity(vm[apiliteral::option::entity].as<std::string>());
  }

  if (vm.count(apiliteral::option::configuration)) {
    configuration(vm[apiliteral::option::configuration].as<std::string>());
  }

  return process_exit_code::SUCCESS;
}

bpo::options_description ManageConfigsOperation::makeProgramOptions() const {
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

JsonData ManageConfigsOperation::writeJsonData() const {
  using namespace artdaq::database::json;

  auto docAST = object_t{};

  if (!JsonReader{}.read(OperationBase::writeJsonData().json_buffer, docAST)) {
    throw db::invalid_option_exception("ManageConfigsOperation") << "Unable to readquery_filter_to_JsonData().";
  }

  if (configuration() != jsonliteral::notprovided) docAST[apiliteral::option::configuration] = configuration();

  auto json_buffer = std::string{};

  if (!JsonWriter{}.write(docAST, json_buffer)) {
    throw db::invalid_option_exception("ManageConfigsOperation") << "Unable to write JSON buffer.";
  }
  return {json_buffer};
}

JsonData ManageConfigsOperation::query_filter_to_JsonData() const {
  if (queryFilter() != jsonliteral::notprovided) {
    return {queryFilter()};
  }

  using namespace artdaq::database::json;

  auto docAST = object_t{};

  if (!JsonReader{}.read(OperationBase::query_filter_to_JsonData().json_buffer, docAST)) {
    throw db::invalid_option_exception("ManageConfigsOperation") << "Unable to query_filter_to_JsonData().";
  }

  if (version() != jsonliteral::notprovided) docAST[apiliteral::filter::version] = version();

  if (entity() != jsonliteral::notprovided) docAST[apiliteral::filter::entity] = entity();

  if (configuration() != jsonliteral::notprovided && operation() != apiliteral::operation::addconfig)
    docAST[apiliteral::filter::configuration] = configuration();

  if (docAST.empty()) {
    return {apiliteral::empty_json};
  }

  auto json_buffer = std::string{};

  if (!JsonWriter{}.write(docAST, json_buffer)) {
    throw db::invalid_option_exception("ManageConfigsOperation") << "Unable to write JSON buffer.";
  }
  return {json_buffer};
}

JsonData ManageConfigsOperation::configuration_to_JsonData() const {
  using namespace artdaq::database::json;
  auto docAST = object_t{};

  docAST[apiliteral::filter::configuration] = configuration();

  auto json_buffer = std::string{};

  if (!JsonWriter{}.write(docAST, json_buffer)) {
    throw db::invalid_option_exception("ManageConfigsOperation") << "Unable to write JSON buffer.";
  }

  return {json_buffer};
}

JsonData ManageConfigsOperation::version_to_JsonData() const {
  using namespace artdaq::database::json;
  auto docAST = object_t{};

  docAST[apiliteral::filter::version] = version();

  auto json_buffer = std::string{};

  if (!JsonWriter{}.write(docAST, json_buffer)) {
    throw db::invalid_option_exception("ManageConfigsOperation") << "Unable to write JSON buffer.";
  }

  return {json_buffer};
}

JsonData ManageConfigsOperation::entity_to_JsonData() const {
  using namespace artdaq::database::json;
  auto docAST = object_t{};

  docAST[apiliteral::name] = entity();

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

  TRACE_(0,
         "artdaq::database::configuration::options::OperationFindConfigs "
         "trace_enable");
}
