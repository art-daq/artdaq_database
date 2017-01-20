#include "test/common.h"

#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/ConfigurationDB/configuration_common.h"
#include "artdaq-database/ConfigurationDB/dispatch_common.h"

#include "artdaq-database/JsonDocument/JSONDocument.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h"
#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb_index.h"

#include "artdaq-database/StorageProviders/MongoDB/provider_mongodb.h"

#include "artdaq-database/DataFormats/Json/json_reader.h"

namespace db = artdaq::database;
namespace cf = db::configuration;
namespace cfo = cf::options;

namespace bpo = boost::program_options;

using Options = cf::ManageConfigsOperation;

using artdaq::database::docrecord::JSONDocument;
using artdaq::database::basictypes::JsonData;

namespace artdaq {
namespace database {
std::string expand_environment_variables(std::string var);
}
}

int main(int argc, char* argv[]) try {
#if 1
  artdaq::database::filesystem::debug::enable();
  artdaq::database::mongo::debug::enable();
  artdaq::database::docrecord::debug::enableJSONDocument();
  artdaq::database::docrecord::debug::enableJSONDocumentBuilder();

  artdaq::database::configuration::debug::enableFindConfigsOperation();
  artdaq::database::configuration::debug::enableCreateConfigsOperation();

  artdaq::database::configuration::debug::options::enableOperationBase();
  artdaq::database::configuration::debug::options::enableOperationManageConfigs();
  artdaq::database::configuration::debug::detail::enableCreateConfigsOperation();
  artdaq::database::configuration::debug::detail::enableFindConfigsOperation();

  artdaq::database::configuration::debug::enableDBOperationMongo();
  artdaq::database::configuration::debug::enableDBOperationFileSystem();

  debug::registerUngracefullExitHandlers();
  artdaq::database::useFakeTime(true);
  artdaq::database::filesystem::index::debug::enable();
#endif

  artdaq::database::filesystem::index::shouldAutoRebuildSearchIndex(true);
  auto options = Options{argv[0]};
  auto desc = options.makeProgramOptions();

  bpo::variables_map vm;

  try {
    bpo::store(bpo::command_line_parser(argc, argv).options(desc).run(), vm);
    bpo::notify(vm);
  } catch (bpo::error const& e) {
    std::cerr << "Exception from command line processing in " << argv[0] << ": " << e.what() << "\n";
    return process_exit_code::INVALID_ARGUMENT;
  }

  if (vm.count("help")) {
    std::cerr << desc << std::endl;
    return process_exit_code::HELP;
  }

  auto exit_code = options.readProgramOptions(vm);
  if (exit_code != process_exit_code::SUCCESS) {
    std::cerr << desc << std::endl;
    return exit_code;
  }

  auto file_res_name = std::string{"${HOME}/${0}.result.out"};

  if (options.operation().compare(apiliteral::operation::readdocument) == 0 || options.operation().compare(apiliteral::operation::globalconfload) == 0) {
    if (!vm.count(apiliteral::option::result)) {
      std::cerr << "Exception from command line processing in " << argv[0] << ": no result file name given.\n"
                << "For usage and an options list, please do '" << argv[0] << " --help"
                << "'.\n";

      return process_exit_code::INVALID_ARGUMENT | 1;
    }

    file_res_name = vm[apiliteral::option::result].as<std::string>();
  }

  file_res_name = artdaq::database::expand_environment_variables(file_res_name);
  file_res_name.pop_back();

  auto file_src_name = file_res_name;

  if (vm.count(apiliteral::option::source)) {
    file_src_name = artdaq::database::expand_environment_variables(vm[apiliteral::option::source].as<std::string>());
    file_src_name.pop_back();
  }

  auto options_string = options.to_string();
  auto test_document = std::string{};

  std::cout << "Parsed options:\n" << options_string << "\n";

  using namespace artdaq::database::configuration::json;

  cf::registerOperation<cf::opsig_str_rstr_t, cf::opsig_str_rstr_t::FP, std::string const&, std::string&>(
      apiliteral::operation::readdocument, read_document, options_string, test_document);

  cf::registerOperation<cf::opsig_str_rstr_t, cf::opsig_str_rstr_t::FP, std::string const&, std::string&>(
      apiliteral::operation::globalconfload, read_configuration, options_string, test_document);

  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(apiliteral::operation::findconfigs, find_configurations,
                                                                                  options_string);
  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(apiliteral::operation::confcomposition,
                                                                                  configuration_composition, options_string);
  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(apiliteral::operation::addconfig,
                                                                                  assign_configuration, options_string);
  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(apiliteral::operation::findversions, find_versions,
                                                                                  options_string);
  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(apiliteral::operation::findentities, find_entities,
                                                                                  options_string);
  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(apiliteral::operation::addalias, add_version_alias, options_string);
  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(apiliteral::operation::rmalias, remove_version_alias, options_string);
  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(apiliteral::operation::findalias, find_version_aliases,
                                                                                  options_string);

  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(apiliteral::operation::listdatabases, list_databases,
                                                                                  options_string);

  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(apiliteral::operation::listcollections, list_collections,
                                                                                  options_string);

  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(apiliteral::operation::readdbinfo, read_dbinfo, options_string);

  try {
    std::ifstream is(file_src_name);
    test_document = std::string((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
    is.close();
    cf::registerOperation<cf::opsig_strstr_t, cf::opsig_strstr_t::FP, std::string const&, std::string const&>(
        apiliteral::operation::writedocument, write_document, options_string, test_document);

    cf::registerOperation<cf::opsig_strstr_t, cf::opsig_strstr_t::FP, std::string const&, std::string const&>(
        apiliteral::operation::globalconfstore, write_configuration, options_string, test_document);

  } catch (...) {
  }

  std::cout << "Running :<" << options.operation() << ">\n";

  auto result = cf::getOperations().at(options.operation())->invoke();

  if (!result.first) {
    std::cout << "Failed; error message: " << result.second << "\n";
    std::cout << debug::current_exception_diagnostic_information();
    return process_exit_code::FAILURE;
  }

  auto returned = std::string{result.second};

  if (options.operation().compare(apiliteral::operation::readdocument) == 0 || options.operation().compare(apiliteral::operation::globalconfload) == 0) {
    std::ofstream os(file_res_name.c_str());
    std::copy(returned.begin(), returned.end(), std::ostream_iterator<char>(os));
    os.close();

    std::cout << "Wrote file:" << file_res_name << "\n";

    return process_exit_code::SUCCESS;
  } else if (options.operation().compare(apiliteral::operation::writedocument) == 0 || options.operation().compare(apiliteral::operation::globalconfstore) == 0) {
    /* std::ofstream os(file_res_name.c_str());
     std::copy(returned.begin(), returned.end(),
             std::ostream_iterator<char>(os));
     os.close();

     std::cout << "Wrote responce to file:" << file_res_name << "\n";
     */
    return process_exit_code::SUCCESS;
  }

  std::cout << "Results(Returned value):\n" << returned << "\n";
  return process_exit_code::SUCCESS;

} catch (...) {
  std::cout << "Process exited with error: " << boost::current_exception_diagnostic_information();
  return process_exit_code::UNCAUGHT_EXCEPTION;
}

/*

rm -rf ~/databases/filesystemdb*

export MY_PROJECT_SRC_DIR="/home/lukhanin/scratch/current/artdaq-demo-base"
export
MY_TEST_FILES_DIR=${MY_PROJECT_SRC_DIR}/artdaq-utilities-database/test/CaseStudy/ArtdaqDemo/test007
export MY_TMP_FILES_DIR=${MY_PROJECT_SRC_DIR}/tmp
mkdir -p ${MY_TMP_FILES_DIR}

conftool -d filesystem -f fhicl -o store -v ver01 -g mytest01 -e agg1  -c
artdaq_Aggregator -s ${MY_TEST_FILES_DIR}/Aggregator_topol.fnal.gov_5265.src.fcl
conftool -d filesystem -f fhicl -o store -v ver01 -g mytest01 -e agg2  -c
artdaq_Aggregator -s ${MY_TEST_FILES_DIR}/Aggregator_topol.fnal.gov_5266.src.fcl
conftool -d filesystem -f fhicl -o store -v ver01 -g mytest01 -e br1   -c
artdaq_BoardReader -s
${MY_TEST_FILES_DIR}/BoardReader_TOY1_topol.fnal.gov_5205.src.fcl
conftool -d filesystem -f fhicl -o store -v ver01 -g mytest01 -e br2   -c
artdaq_BoardReader -s
${MY_TEST_FILES_DIR}/BoardReader_TOY2_topol.fnal.gov_5206.src.fcl
conftool -d filesystem -f fhicl -o store -v ver01 -g mytest01 -e eb1   -c
artdaq_EventBuilder -s
${MY_TEST_FILES_DIR}/EventBuilder_topol.fnal.gov_5235.src.fcl
conftool -d filesystem -f fhicl -o store -v ver01 -g mytest01 -e eb2   -c
artdaq_EventBuilder -s
${MY_TEST_FILES_DIR}/EventBuilder_topol.fnal.gov_5236.src.fcl

conftool -d filesystem -f gui -o findconfigs
conftool -d filesystem -f gui -o buildfilter
conftool -d filesystem -f gui -o findentities


conftool -d filesystem -f fhicl -o load -v ver01 -g mytest01 -e agg1  -c
artdaq_Aggregator -r ${MY_TMP_FILES_DIR}/Aggregator_topol.fnal.gov_5265.src.fcl
conftool -d filesystem -f fhicl -o load -v ver01 -g mytest01 -e agg2  -c
artdaq_Aggregator -r ${MY_TMP_FILES_DIR}/Aggregator_topol.fnal.gov_5266.src.fcl
conftool -d filesystem -f fhicl -o load -v ver01 -g mytest01 -e br1   -c
artdaq_BoardReader -r
${MY_TMP_FILES_DIR}/BoardReader_TOY1_topol.fnal.gov_5205.src.fcl
conftool -d filesystem -f fhicl -o load -v ver01 -g mytest01 -e br2   -c
artdaq_BoardReader -r
${MY_TMP_FILES_DIR}/BoardReader_TOY2_topol.fnal.gov_5206.src.fcl
conftool -d filesystem -f fhicl -o load -v ver01 -g mytest01 -e eb1   -c
artdaq_EventBuilder -r
${MY_TMP_FILES_DIR}/EventBuilder_topol.fnal.gov_5235.src.fcl
conftool -d filesystem -f fhicl -o load -v ver01 -g mytest01 -e eb2   -c
artdaq_EventBuilder -r
${MY_TMP_FILES_DIR}/EventBuilder_topol.fnal.gov_5236.src.fcl

 */