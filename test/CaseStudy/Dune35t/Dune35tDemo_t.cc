#include "test/common.h"

#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/ConfigurationDB/configuration_common.h"
#include "artdaq-database/ConfigurationDB/dispatch_common.h"

#include "artdaq-database/JsonDocument/JSONDocument.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h"
#include "artdaq-database/StorageProviders/MongoDB/provider_mongodb.h"

#include "artdaq-database/DataFormats/Json/json_reader.h"

namespace db = artdaq::database;
namespace cf = db::configuration;
namespace cfo = cf::options;

namespace bpo = boost::program_options;

using Options = cf::ManageDocumentOperation;

using artdaq::database::docrecord::JSONDocument;
using artdaq::database::basictypes::JsonData;

int main(int argc, char* argv[]) try {
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

  if (!vm.count(apiliteral::option::result)) {
    std::cerr << "Exception from command line processing in " << argv[0] << ": no result file name given.\n"
              << "For usage and an options list, please do '" << argv[0] << " --help"
              << "'.\n";

    return process_exit_code::INVALID_ARGUMENT | 1;
  }

  auto file_res_name = vm[apiliteral::option::result].as<std::string>();

  auto file_src_name = file_res_name;

  if (vm.count(apiliteral::option::source)) {
    file_src_name = vm[apiliteral::option::source].as<std::string>();
  }

  auto options_string = options.to_string();
  auto test_document = std::string{};

  std::cout << "Parsed options:\n" << options_string << "\n";

  using namespace artdaq::database::configuration::json;

  cf::registerOperation<cf::opsig_str_rstr_t, cf::opsig_str_rstr_t::FP, std::string const&, std::string&>(
      apiliteral::operation::readdocument, read_document, options_string, test_document);

  cf::registerOperation<cf::opsig_str_rstr_t, cf::opsig_str_rstr_t::FP, std::string const&, std::string&>(
      apiliteral::operation::readconfiguration, read_configuration, options_string, test_document);

  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(apiliteral::operation::findconfigs,
                                                                                  find_configurations, options_string);
  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(
      apiliteral::operation::confcomposition, configuration_composition, options_string);
  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(apiliteral::operation::assignconfig,
                                                                                  assign_configuration, options_string);
  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(apiliteral::operation::findversions,
                                                                                  find_versions, options_string);
  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(apiliteral::operation::findentities,
                                                                                  find_entities, options_string);
  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(apiliteral::operation::addversionalias,
                                                                                  add_version_alias, options_string);
  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(apiliteral::operation::rmversionalias,
                                                                                  remove_version_alias, options_string);
  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(apiliteral::operation::findversionalias,
                                                                                  find_version_aliases, options_string);

  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(apiliteral::operation::listdatabases,
                                                                                  list_databases, options_string);

  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(
      apiliteral::operation::listcollections, list_collections, options_string);

  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(apiliteral::operation::readdbinfo,
                                                                                  read_dbinfo, options_string);

  try {
    std::ifstream is(file_src_name);
    test_document = std::string((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
    is.close();
    cf::registerOperation<cf::opsig_strstr_t, cf::opsig_strstr_t::FP, std::string const&, std::string const&>(
        apiliteral::operation::writedocument, write_document, options_string, test_document);

    cf::registerOperation<cf::opsig_strstr_t, cf::opsig_strstr_t::FP, std::string const&, std::string const&>(
        apiliteral::operation::writeconfiguration, write_configuration, options_string, test_document);

  } catch (...) {
  }

  std::cout << "Running test:<" << options.operation() << ">\n";

  auto result = cf::getOperations().at(options.operation())->invoke();

  if (!result.first) {
    std::cout << "Test failed; error message: " << result.second << "\n";
    std::cout << debug::current_exception_diagnostic_information();
    return process_exit_code::FAILURE;
  }

  auto returned = std::string{result.second};

  std::ifstream is(file_res_name);
  auto expected = std::string((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
  is.close();

  using cfo::data_format_t;

  if (options.format() == data_format_t::gui || options.format() == data_format_t::db ||
      options.format() == data_format_t::json) {
    auto compare_result = artdaq::database::json::compare_json_objects(returned, expected);
    if (compare_result.first) {
      std::cout << "returned:\n" << returned << "\n";

      return process_exit_code::SUCCESS;
    }
    std::cout << "Test failed (expected!=returned); error message: " << compare_result.second << "\n";
  } else if (expected.compare(returned) == 0) {
    std::cout << "returned:\n" << returned << "\n";

    return process_exit_code::SUCCESS;
  } else if (expected.pop_back(), expected.compare(returned) == 0) {
    std::cout << "returned:\n" << returned << "\n";

    return process_exit_code::SUCCESS;
  }

  std::cout << "Test failed (expected!=returned)\n";

  std::cout << "returned:\n" << returned << "\n";
  std::cout << "expected:\n" << expected << "\n";

  auto mismatch = std::mismatch(expected.begin(), expected.end(), returned.begin());
  std::cout << "File sizes (exp,ret)=(" << std::distance(expected.begin(), expected.end()) << ","
            << std::distance(returned.begin(), returned.end()) << ")\n";

  std::cout << "First mismatch at position " << std::distance(expected.begin(), mismatch.first) << ", (exp,ret)=(0x"
            << std::hex << (unsigned int)*mismatch.first << ",0x" << (unsigned int)*mismatch.second << ")\n";

  auto file_out_name = std::string(db::filesystem::mkdir(tmpdir))
                           .append(argv[0])
                           .append("-")
                           .append(options.operation())
                           .append("-")
                           .append(basename((char*)file_src_name.c_str()))
                           .append(".txt");

  std::ofstream os(file_out_name.c_str());
  std::copy(returned.begin(), returned.end(), std::ostream_iterator<char>(os));
  os.close();

  std::cout << "Wrote file:" << file_out_name << "\n";

  return process_exit_code::FAILURE;
} catch (...) {
  std::cout << "Process exited with error: " << boost::current_exception_diagnostic_information();
  return process_exit_code::UNCAUGHT_EXCEPTION;
}
