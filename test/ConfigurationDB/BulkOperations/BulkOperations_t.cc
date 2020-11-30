#include "test/common.h"

#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/ConfigurationDB/configuration_common.h"
#include "artdaq-database/ConfigurationDB/dispatch_common.h"
#include "artdaq-database/ConfigurationDB/options_operations.h"

#include "artdaq-database/JsonDocument/JSONDocument.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h"
#include "artdaq-database/StorageProviders/MongoDB/provider_mongodb.h"

#include "artdaq-database/DataFormats/Json/json_reader.h"

namespace db = artdaq::database;
namespace cf = db::configuration;
namespace cfo = cf::options;

namespace bpo = boost::program_options;

using Options = cf::BulkOperations;

int main(int argc, char* argv[]) try {
  artdaq::database::filesystem::debug::enable();
  artdaq::database::mongo::debug::enable();
  // artdaq::database::docrecord::debug::JSONDocument();
  artdaq::database::docrecord::debug::JSONDocumentBuilder();

  artdaq::database::configuration::debug::ManageConfigs();
  artdaq::database::configuration::debug::ManageAliases();
  artdaq::database::configuration::debug::ManageDocuments();

  artdaq::database::configuration::debug::options::OperationBase();
  artdaq::database::configuration::debug::options::ManageDocuments();
  artdaq::database::configuration::debug::options::ManageConfigs();
  artdaq::database::configuration::debug::options::ManageAliases();

  artdaq::database::configuration::debug::detail::ManageConfigs();
  artdaq::database::configuration::debug::detail::ManageAliases();
  artdaq::database::configuration::debug::detail::ManageDocuments();

  artdaq::database::configuration::debug::MongoDB();
  artdaq::database::configuration::debug::UconDB();

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

  if (vm.count("help") != 0u) {
    std::cerr << desc << std::endl;
    return process_exit_code::HELP;
  }

  auto exit_code = options.readProgramOptions(vm);
  if (exit_code != process_exit_code::SUCCESS) {
    std::cerr << desc << std::endl;
    return exit_code;
  }

  if (vm.count(apiliteral::option::result) == 0u) {
    std::cerr << "Exception from command line processing in " << argv[0] << ": no result file name given.\n"
              << "For usage and an options list, please do '" << argv[0] << " --help"
              << "'.\n";

    return process_exit_code::INVALID_ARGUMENT | 1;
  }

  auto file_res_name = vm[apiliteral::option::result].as<std::string>();

  auto options_string = options.to_string();
  auto operation_name = std::string{"newconfig"};

  std::cout << "Parsed options:\n" << options_string << "\n";

  using namespace artdaq::database::configuration::json;

  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(operation_name, create_configuration, options_string);

  std::cout << "Running test:<" << operation_name << ">\n";

  auto result = cf::getOperations().at(operation_name)->invoke();

  if (!result.first) {
    std::cout << "Test failed; error message: " << result.second << "\n";
    std::cout << ::debug::current_exception_diagnostic_information();
    return process_exit_code::FAILURE;
  }

  if (result.second.empty()) {
    std::cout << "Test failed; error message: result is empty."
              << "\n";
    return process_exit_code::FAILURE;
  }

  auto returned = std::string{result.second};

  auto expected = std::string{};
  db::read_buffer_from_file(expected, file_res_name);

  using cfo::data_format_t;

  if (options.format() == data_format_t::gui || options.format() == data_format_t::db || options.format() == data_format_t::json) {
    auto compare_result = artdaq::database::json::compare_json_objects(returned, expected);
    if (compare_result.first) {
      std::cout << "returned:\n" << returned << "\n";

      return process_exit_code::SUCCESS;
    }
    std::cout << "Test failed (expected!=returned); error message: " << compare_result.second << "\n";
  } else if (expected == returned) {
    std::cout << "returned:\n" << returned << "\n";

    return process_exit_code::SUCCESS;
  } else if (expected.pop_back(), expected == returned) {
    std::cout << "returned:\n" << returned << "\n";

    return process_exit_code::SUCCESS;
  }

  std::cout << "Test failed (expected!=returned)\n";

  std::cout << "returned:\n" << returned << "\n";
  std::cout << "expected:\n" << expected << "\n";

  auto mismatch = std::mismatch(expected.begin(), expected.end(), returned.begin());
  std::cout << "File sizes (exp,ret)=(" << std::distance(expected.begin(), expected.end()) << "," << std::distance(returned.begin(), returned.end())
            << ")\n";

  std::cout << "First mismatch at position " << std::distance(expected.begin(), mismatch.first) << ", (exp,ret)=(0x" << std::hex
            << static_cast<unsigned int>(*mismatch.first) << ",0x" << static_cast<unsigned int>(*mismatch.second) << ")\n";

  auto file_out_name = std::string(db::filesystem::mkdir(tmpdir))
                           .append("/")
                           .append(argv[0])
                           .append("-")
                           .append(operation_name)
                           .append("-")
                           .append(basename(const_cast<char*>(file_res_name.c_str())))
                           .append(".txt");

  db::write_buffer_to_file(returned, file_out_name);

  std::cout << "Wrote file:" << file_out_name << "\n";

  return process_exit_code::FAILURE;
} catch (...) {
  std::cout << "Process exited with error: " << ::debug::current_exception_diagnostic_information();
  return process_exit_code::UNCAUGHT_EXCEPTION;
}
