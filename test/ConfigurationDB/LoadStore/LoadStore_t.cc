#include "test/common.h"

#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/ConfigurationDB/configuration_common.h"

#include "artdaq-database/JsonDocument/JSONDocument.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h"
#include "artdaq-database/StorageProviders/MongoDB/provider_mongodb.h"

#include "artdaq-database/DataFormats/Json/json_reader.h"
#include "artdaq-database/DataFormats/common/helper_functions.h"
#include "artdaq-database/DataFormats/common/shared_literals.h"

namespace db = artdaq::database;
namespace cf = db::configuration;
namespace cfo = cf::options;

namespace literal = artdaq::database::configuration::literal;
namespace bpo = boost::program_options;

using Options = cf::LoadStoreOperation;

using artdaq::database::jsonutils::JSONDocument;
using artdaq::database::basictypes::JsonData;

typedef bool (*test_case)(Options const& /*options*/, std::string const& /*file_name*/);

bool test_findconfigs(Options const&, std::string const&);
bool test_buildfilter(Options const&, std::string const&);

int main(int argc, char* argv[]) try {
  artdaq::database::filesystem::debug::enable();
  artdaq::database::mongo::debug::enable();
  //artdaq::database::jsonutils::debug::enableJSONDocument();
  //artdaq::database::jsonutils::debug::enableJSONDocumentBuilder();

  artdaq::database::configuration::debug::enableLoadStoreOperation();
  artdaq::database::configuration::debug::options::enableOperationBase();
  artdaq::database::configuration::debug::options::enableLoadStoreOperation();
  artdaq::database::configuration::debug::detail::enableLoadStoreOperation();

  debug::registerUngracefullExitHandlers();
  artdaq::database::dataformats::useFakeTime(true);

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

  if (!vm.count(literal::option::result)) {
    std::cerr << "Exception from command line processing in " << argv[0] << ": no result file name given.\n"
              << "For usage and an options list, please do '" << argv[0] << " --help"
              << "'.\n";

    return process_exit_code::INVALID_ARGUMENT | 1;
  }

  auto file_res_name = vm[literal::option::result].as<std::string>();

  auto file_src_name = file_res_name;

  if (vm.count(literal::option::source)) {
    file_src_name = vm[literal::option::source].as<std::string>();
  }

  auto options_string = options.to_string();

  std::cout << "Parsed options:\n" << options_string << "\n";

  using namespace artdaq::database::configuration::json;

  auto test_document = std::string{};

  cf::registerOperation<cf::opsig_str_rstr_t, cf::opsig_str_rstr_t::FP, std::string const&, std::string&>(
      literal::operation::load, load_configuration, options_string, test_document);

  try {
    std::ifstream is(file_src_name);
    test_document = std::string((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
    is.close();
    cf::registerOperation<cf::opsig_strstr_t, cf::opsig_strstr_t::FP, std::string const&, std::string const&>(
        literal::operation::store, store_configuration, options_string, test_document);
  } catch (...) {
  }

  std::cout << "Running test:<" << options.operation() << ">\n";

  auto result = cf::getOperations().at(options.operation())->invoke();

  if (!result.first) {
    std::cout << "Test failed; error message: " << result.second << "\n";
    return process_exit_code::FAILURE;
  }

  auto returned = result.second;

  std::ifstream is(file_res_name);
  auto expected = std::string((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
  is.close();

  using cfo::data_format_t;

  if (options.dataFormat() == data_format_t::gui || options.dataFormat() == data_format_t::db ||
      options.dataFormat() == data_format_t::json) {
    auto compare_result = artdaq::database::json::compare_json_objects(returned, expected);
    if (compare_result.first) {
      std::cout << "returned:\n" << returned << "\n";

      return process_exit_code::SUCCESS;
    }
    std::cout << "Test failed (expected!=returned); error message: " << compare_result.second << "\n";
  } else if (returned == expected) {
    std::cout << "returned:\n" << returned << "\n";

    return process_exit_code::SUCCESS;
  } else {
    std::cout << "Test failed (expected!=returned)\n";
  }

  std::cout << "returned:\n" << returned << "\n";
  std::cout << "expected:\n" << expected << "\n";

  auto file_out_name = std::string(artdaq::database::mkdir(tmpdir))
                           .append(argv[0])
                           .append("-")
                           .append(options.operation())
                           .append("-")
                           .append(basename((char*)file_src_name.c_str()))
                           .append(".txt");

  std::ofstream os(file_out_name.c_str());
  std::copy(result.second.begin(), result.second.end(), std::ostream_iterator<char>(os));
  os.close();

  std::cout << "Wrote file:" << file_out_name << "\n";

  return process_exit_code::FAILURE;
} catch (...) {
  std::cout << "Process exited with error: " << boost::current_exception_diagnostic_information();
  return process_exit_code::UNCAUGHT_EXCEPTION;
}
