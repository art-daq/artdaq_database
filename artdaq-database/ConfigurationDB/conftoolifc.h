#ifndef _ARTDAQ_DATABASE_CONFTOOLIFC_H_
#define _ARTDAQ_DATABASE_CONFTOOLIFC_H_

#include "artdaq-database/ConfigurationDB/common.h"
#include "artdaq-database/ConfigurationDB/configuration_common.h"

namespace db = artdaq::database;
namespace cf = db::configuration;
namespace cfo = cf::options;
namespace bpo = boost::program_options;

namespace db = artdaq::database;
namespace jsonliteral = db::dataformats::literal;
namespace apiliteral = db::configapi::literal;

namespace artdaq {
namespace database {
std::string expand_environment_variables(const std::string& var);

template <typename OPTS>
result_t conftool_impl(std::unique_ptr<OPTS>& options, int argc, char* argv[]) {
  db::set_default_locale();

  auto desc = options->makeProgramOptions();

  bpo::variables_map vm;

  try {
    bpo::store(bpo::command_line_parser(argc, argv).options(desc).run(), vm);
    bpo::notify(vm);
  } catch (bpo::error const& e) {
    std::cerr << "Exception from command line processing in " << options->processName() << ": " << e.what() << "\n";
    return Failure("process_exit_code::INVALID_ARGUMENT");
  }

  if (vm.count("help")) {
    std::cerr << desc << std::endl;
    return Success("process_exit_code::HELP");
  }

  auto exit_code = options->readProgramOptions(vm);
  if (exit_code != process_exit_code::SUCCESS) {
    std::cerr << desc << std::endl;
    return Failure("process_exit_code::"s + std::to_string(exit_code));
  }

  if (options->operation().compare(apiliteral::operation::readdocument) == 0 ||
      options->operation().compare(apiliteral::operation::readconfiguration) == 0) {
    if (!vm.count(apiliteral::option::result)) {
      std::cerr << "Exception from command line processing in " << options->processName() << ": no result file name given.\n"
                << "For usage and an options list, please do '" << options->processName() << " --help"
                << "'.\n";

      return Failure("process_exit_code::INVALID_ARGUMENT | 1");
    }
  }

  auto file_src_name = options->resultFileName();

  if (vm.count(apiliteral::option::source)) {
    file_src_name = artdaq::database::expand_environment_variables(vm[apiliteral::option::source].as<std::string>());
  }

  auto options_string = options->to_string();
  auto test_document = std::string{};

  using namespace artdaq::database::configuration::json;

  cf::registerOperation<cf::opsig_str_rstr_t, cf::opsig_str_rstr_t::FP, std::string const&, std::string&>(
      apiliteral::operation::readdocument, read_document, options_string, test_document);

  cf::registerOperation<cf::opsig_str_rstr_t, cf::opsig_str_rstr_t::FP, std::string const&, std::string&>(
      apiliteral::operation::readconfiguration, read_configuration, options_string, test_document);

  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(apiliteral::operation::findconfigs, find_configurations,
                                                                                  options_string);
  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(apiliteral::operation::confcomposition, configuration_composition,
                                                                                  options_string);
  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(apiliteral::operation::assignconfig, assign_configuration,
                                                                                  options_string);
  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(apiliteral::operation::removeconfig, remove_configuration,
                                                                                  options_string);

  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(apiliteral::operation::searchcollection, search_collection,
                                                                                  options_string);

  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(apiliteral::operation::findversions, find_versions, options_string);
  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(apiliteral::operation::findruns, find_runs, options_string);
  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(apiliteral::operation::findentities, find_entities, options_string);
  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(apiliteral::operation::addentity, add_entity, options_string);
  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(apiliteral::operation::rmentity, remove_entity, options_string);
  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(apiliteral::operation::markdeleted, mark_document_deleted,
                                                                                  options_string);
  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(apiliteral::operation::markreadonly, mark_document_readonly,
                                                                                  options_string);

  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(apiliteral::operation::findversionalias, find_version_aliases,
                                                                                  options_string);
  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(apiliteral::operation::addversionalias, add_version_alias,
                                                                                  options_string);
  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(apiliteral::operation::rmversionalias, remove_version_alias,
                                                                                  options_string);

  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(apiliteral::operation::listdatabases, list_databases,
                                                                                  options_string);

  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(apiliteral::operation::listcollections, list_collections,
                                                                                  options_string);

  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(apiliteral::operation::readdbinfo, read_dbinfo, options_string);

  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(apiliteral::operation::exportcollection, export_collection,
                                                                                  options_string);
  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(apiliteral::operation::importcollection, import_collection,
                                                                                  options_string);

  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(apiliteral::operation::exportdatabase, export_database,
                                                                                  options_string);
  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(apiliteral::operation::importdatabase, import_database,
                                                                                  options_string);

  try {
    db::read_buffer_from_file(test_document, file_src_name);

    cf::registerOperation<cf::opsig_strstr_t, cf::opsig_strstr_t::FP, std::string const&, std::string const&>(
        apiliteral::operation::writedocument, write_document, options_string, test_document);

    cf::registerOperation<cf::opsig_strstr_t, cf::opsig_strstr_t::FP, std::string const&, std::string const&>(
        apiliteral::operation::writeconfiguration, write_configuration, options_string, test_document);

  } catch (...) {
  }

  if (cf::getOperations().count(options->operation()) == 0) {
    std::cerr << "Unsupported operation \"" << options->operation() << "\"\n";
    return Failure("Unsupported operation "s + options->operation());
  }

  return cf::getOperations().at(options->operation())->invoke();
}
}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_CONFTOOLIFC_H_ */
