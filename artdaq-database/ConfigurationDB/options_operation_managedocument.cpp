#include "artdaq-database/ConfigurationDB/common.h"

#include "artdaq-database/ConfigurationDB/options_operation_base.h"
#include "artdaq-database/ConfigurationDB/options_operation_managedocument.h"

#include "artdaq-database/ConfigurationDB/shared_helper_functions.h"

#include "artdaq-database/DataFormats/shared_literals.h"

#include "artdaq-database/BasicTypes/basictypes.h"

#include "artdaq-database/DataFormats/Json/json_reader.h"
#include "artdaq-database/DataFormats/Json/json_writer.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "options_operation_managedocument.cpp"

namespace db = artdaq::database;
namespace cf = db::configuration;
namespace dbbt = db::basictypes;

using cf::ManageDocumentOperation;
using cf::OperationBase;
using dbbt::JsonData;

ManageDocumentOperation::ManageDocumentOperation(std::string const& process_name) : OperationBase{process_name} {}

std::string const& ManageDocumentOperation::version() const {
  confirm(!_version.empty());

  return _version;
}

std::string const& ManageDocumentOperation::version(std::string const& version) {
  confirm(!version.empty());

  if (version.empty()) {
    throw runtime_error("Options") << "Invalid version; version is empty.";
  }

  TLOG(20) << "Options: Updating version from " << _version << " to " << version << ".";

  _version = version;

  return _version;
}

std::string const& ManageDocumentOperation::run() const {
  confirm(!_run.empty());

  return _run;
}

std::string const& ManageDocumentOperation::run(std::string const& run) {
  confirm(!run.empty());

  if (run.empty()) {
    throw runtime_error("Options") << "Invalid run; run is empty.";
  }

  TLOG(20) << "Options: Updating run from " << _run << " to " << run << ".";

  _run = run;

  return _run;
}
std::string const& ManageDocumentOperation::entity() const {
  confirm(!_entity.empty());

  return _entity;
}

std::string const& ManageDocumentOperation::entity(std::string const& entity) {
  confirm(!entity.empty());

  if (entity.empty()) {
    throw runtime_error("Options") << "Invalid configurable entity; entity is empty.";
  }

  TLOG(21) << "Options: Updating entity from " << _entity << " to " << entity << ".";

  _entity = entity;

  return _entity;
}

std::string const& ManageDocumentOperation::configuration() const {
  confirm(!_configuration.empty());

  return _configuration;
}

std::string const& ManageDocumentOperation::configuration(std::string const& global_configuration) {
  confirm(!global_configuration.empty());

  if (global_configuration.empty()) {
    throw runtime_error("Options") << "Invalid global config; global config is empty.";
  }

  TLOG(22) << "Options: Updating global_configuration from " << _configuration << " to " << global_configuration << ".";

  _configuration = global_configuration;

  return _configuration;
}

std::string const& ManageDocumentOperation::sourceFileName() const {
  confirm(!_source_file_name.empty());

  return _source_file_name;
}

std::string const& ManageDocumentOperation::sourceFileName(std::string const& source_file_name) {
  confirm(!source_file_name.empty());

  if (source_file_name.empty()) {
    throw runtime_error("Options") << "Invalid source file name; source file name is empty.";
  }

  TLOG(22) << "Options: Updating source_file_name from " << _source_file_name << " to " << source_file_name << ".";

  _source_file_name = source_file_name;

  return _source_file_name;
}

void ManageDocumentOperation::readJsonData(JsonData const& data) {
  confirm(!data.empty());

  OperationBase::readJsonData(data);
  TLOG(22) << "OperationBase::readJsonData <" << data.json_buffer << ">";

  using namespace artdaq::database::json;
  auto dataAST = object_t{};

  if (!JsonReader{}.read(data.json_buffer, dataAST)) {
    throw db::invalid_option_exception("ManageDocumentOperation") << "ManageDocumentOperation: Unable to read JSON buffer.";
  }

  try {
    configuration(boost::get<std::string>(dataAST.at(apiliteral::option::configuration)));
  } catch (...) {
  }

  try {
    version(boost::get<std::string>(dataAST.at(apiliteral::option::version)));
  } catch (...) {
  }

  try {
    entity(boost::get<std::string>(dataAST.at(apiliteral::option::entity)));
  } catch (...) {
  }

  try {
    run(boost::get<std::string>(dataAST.at(apiliteral::option::run)));
  } catch (...) {
  }

  try {
    sourceFileName(boost::get<std::string>(dataAST.at(apiliteral::option::source)));
  } catch (...) {
  }

  try {
    auto const& filterAST = boost::get<jsn::object_t>(dataAST.at(apiliteral::option::searchfilter));

    if (filterAST.empty()) {
      queryFilter(apiliteral::notprovided);
    }

    try {
      version(boost::get<std::string>(filterAST.at(apiliteral::filter::version)));
    } catch (...) {
    }

    try {
      entity(boost::get<std::string>(filterAST.at(apiliteral::filter::entities)));
    } catch (...) {
    }

    try {
      configuration(boost::get<std::string>(filterAST.at(apiliteral::filter::configurations)));
    } catch (...) {
    }

    try {
      run(boost::get<std::string>(filterAST.at(apiliteral::filter::runs)));
    } catch (...) {
    }

  } catch (...) {
    TLOG(11) << "Options() no filter provided <" << data << ">";
  }
}

int ManageDocumentOperation::readProgramOptions(bpo::variables_map const& vm) {
  auto result = OperationBase::readProgramOptions(vm);

  if (result != process_exit_code::SUCCESS) {
    return result;
  }

  if (vm.count(apiliteral::option::version) != 0u) {
    version(vm[apiliteral::option::version].as<std::string>());
  }

  if (vm.count(apiliteral::option::entity) != 0u) {
    entity(vm[apiliteral::option::entity].as<std::string>());
  }

  if (vm.count(apiliteral::option::run) != 0u) {
    run(vm[apiliteral::option::run].as<std::string>());
  }

  if (vm.count(apiliteral::option::configuration) != 0u) {
    configuration(vm[apiliteral::option::configuration].as<std::string>());
  }

  if (vm.count(apiliteral::option::source) != 0u) {
    sourceFileName(vm[apiliteral::option::source].as<std::string>());
  }

  return process_exit_code::SUCCESS;
}

bpo::options_description ManageDocumentOperation::makeProgramOptions() const {
  auto opts = OperationBase::makeProgramOptions();

  auto make_opt_name = [](auto& long_name, auto& short_name) { return std::string{long_name}.append(",").append(short_name); };

  opts.add_options()(make_opt_name(apiliteral::option::version, "v").c_str(), bpo::value<std::string>(), "Version");
  opts.add_options()(make_opt_name(apiliteral::option::run, "r").c_str(), bpo::value<std::string>(), "Run");

  opts.add_options()(make_opt_name(apiliteral::option::entity, "e").c_str(), bpo::value<std::string>(), "Configurable-entity name");
  opts.add_options()(make_opt_name(apiliteral::option::configuration, "g").c_str(), bpo::value<std::string>(), "Configuration name");

  opts.add_options()(make_opt_name(apiliteral::option::source, "s").c_str(), bpo::value<std::string>(), "Configuration source file name");

  return opts;
}

JsonData ManageDocumentOperation::writeJsonData() const {
  using namespace artdaq::database::json;

  auto docAST = object_t{};

  if (!JsonReader{}.read(OperationBase::writeJsonData(), docAST)) {
    throw db::invalid_option_exception("ManageDocumentOperation") << "Unable to read query_filter_to_JsonData().";
  }

  if (configuration() != apiliteral::notprovided) {
    docAST[apiliteral::option::configuration] = configuration();
  }

  if (sourceFileName() != apiliteral::notprovided) {
    docAST[apiliteral::option::source] = sourceFileName();
  }

  if (entity() != apiliteral::notprovided) {
    docAST[apiliteral::option::entity] = entity();
  }

  if (version() != apiliteral::notprovided) {
    docAST[apiliteral::option::version] = version();
  }

  if (run() != apiliteral::notprovided) {
    docAST[apiliteral::option::run] = run();
  }

  auto json_buffer = std::string{};

  if (!JsonWriter{}.write(docAST, json_buffer)) {
    throw db::invalid_option_exception("ManageDocumentOperation") << "Unable to write JSON buffer.";
  }
  return {json_buffer};
}

JsonData ManageDocumentOperation::query_filter_to_JsonData() const {
  if (queryFilter() != apiliteral::notprovided) {
    return {queryFilter()};
  }

  using namespace artdaq::database::json;

  auto docAST = object_t{};

  if (!JsonReader{}.read(OperationBase::query_filter_to_JsonData(), docAST)) {
    throw db::invalid_option_exception("ManageDocumentOperation") << "Unable to query_filter_to_JsonData().";
  }

  if (version() != apiliteral::notprovided) {
    docAST[apiliteral::filter::version] = version();
  }

  if (entity() != apiliteral::notprovided) {
    docAST[apiliteral::filter::entities] = entity();
  }

  if (configuration() != apiliteral::notprovided && operation() != apiliteral::operation::assignconfig) {
    docAST[apiliteral::filter::configurations] = configuration();
  }

  /*
  if (run() != apiliteral::notprovided)
    docAST[apiliteral::filter::runs] = run();
  */

  if (docAST.empty()) {
    return {apiliteral::empty_json};
  }

  auto json_buffer = std::string{};

  if (!JsonWriter{}.write(docAST, json_buffer)) {
    throw db::invalid_option_exception("ManageDocumentOperation") << "Unable to write JSON buffer.";
  }

  return {json_buffer};
}

JsonData ManageDocumentOperation::configuration_to_JsonData() const {
  using namespace artdaq::database::json;
  auto docAST = object_t{};

  docAST[apiliteral::name] = configuration();

  auto json_buffer = std::string{};

  if (!JsonWriter{}.write(docAST, json_buffer)) {
    throw db::invalid_option_exception("ManageDocumentOperation") << "Unable to write JSON buffer.";
  }

  return {json_buffer};
}

JsonData ManageDocumentOperation::configurationsname_to_JsonData() const {
  using namespace artdaq::database::json;
  auto docAST = object_t{};

  docAST[apiliteral::filter::configurations] = configuration();

  auto json_buffer = std::string{};

  if (!JsonWriter{}.write(docAST, json_buffer)) {
    throw db::invalid_option_exception("ManageDocumentOperation") << "Unable to write JSON buffer.";
  }

  return {json_buffer};
}

JsonData ManageDocumentOperation::collection_to_JsonData() const {
  using namespace artdaq::database::json;
  auto docAST = object_t{};

  docAST[apiliteral::option::collection] = collection();

  auto json_buffer = std::string{};

  if (!JsonWriter{}.write(docAST, json_buffer)) {
    throw db::invalid_option_exception("ManageDocumentOperation") << "Unable to write JSON buffer.";
  }

  return {json_buffer};
}

JsonData ManageDocumentOperation::version_to_JsonData() const {
  using namespace artdaq::database::json;
  auto docAST = object_t{};

  docAST[apiliteral::name] = version();

  auto json_buffer = std::string{};

  if (!JsonWriter{}.write(docAST, json_buffer)) {
    throw db::invalid_option_exception("ManageDocumentOperation") << "Unable to write JSON buffer.";
  }

  return {json_buffer};
}

JsonData ManageDocumentOperation::entity_to_JsonData() const {
  using namespace artdaq::database::json;
  auto docAST = object_t{};

  docAST[apiliteral::name] = entity();

  auto json_buffer = std::string{};

  if (!JsonWriter{}.write(docAST, json_buffer)) {
    throw db::invalid_option_exception("ManageDocumentOperation") << "Unable to write JSON buffer.";
  }

  return {json_buffer};
}

JsonData ManageDocumentOperation::run_to_JsonData() const {
  using namespace artdaq::database::json;
  auto docAST = object_t{};

  docAST[apiliteral::name] = run();

  auto json_buffer = std::string{};

  if (!JsonWriter{}.write(docAST, json_buffer)) {
    throw db::invalid_option_exception("ManageDocumentOperation") << "Unable to write JSON buffer.";
  }

  return {json_buffer};
}

//
void cf::debug::options::ManageDocuments() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);

  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TLOG(10) << "artdaq::database::configuration::options::ManageDocuments trace_enable";
}
