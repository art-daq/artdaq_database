#include "test/common.h"

#include "artdaq-database/ConfigurationDB/common.h"
#include "artdaq-database/ConfigurationDB/conftoolifc.h"

#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/JsonDocument/JSONDocument.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/JsonDocument/JSONDocumentMigrator.h"

#include "artdaq-database/DataFormats/Json/json_reader.h"

namespace db = artdaq::database;
namespace bpo = boost::program_options;

using artdaq::database::docrecord::JSONDocument;
using artdaq::database::docrecord::JSONDocumentBuilder;
using artdaq::database::docrecord::JSONDocumentMigrator;

using test_case = bool (*)(const std::string&, const std::string&);

bool test_migratev1v2(std::string const& /*source*/, std::string const& /*compare*/);

int main(int argc, char* argv[]) try {
#if 0

  impl::enable_trace();

  debug::registerUngracefullExitHandlers();
#endif
  artdaq::database::useFakeTime(true);

  artdaq::database::docrecord::debug::JSONDocumentMigrator();

  // Get the input parameters via the boost::program_options library,
  // designed to make it relatively simple to define arguments and
  // issue errors if argument list is supplied incorrectly

  std::ostringstream descstr;
  descstr << argv[0] << " <-s <source-file>> <-x <compare-with-file>>";

  bpo::options_description desc = descstr.str();

  desc.add_options()("source,s", bpo::value<std::string>(), "Input source file.")(
      "compare,x", bpo::value<std::string>(), "Expected result of convertion.")("help,h", "produce help message");

  bpo::variables_map vm;

  try {
    bpo::store(bpo::command_line_parser(argc, argv).options(desc).run(), vm);
    bpo::notify(vm);
  } catch (bpo::error const& e) {
    std::cerr << "Exception from command line processing in " << argv[0] << ": " << e.what() << "\n";
    return process_exit_code::INVALID_ARGUMENT;
  }

  if (vm.count("help") != 0u) {
    std::cout << desc << std::endl;
    return process_exit_code::HELP;
  }

  if (vm.count("source") == 0u) {
    std::cerr << "Exception from command line processing in " << argv[0] << ": no source file given.\n"
              << "For usage and an options list, please do '" << argv[0] << " --help"
              << "'.\n";
    return process_exit_code::INVALID_ARGUMENT | 1;
  }

  if (vm.count("compare") == 0u) {
    std::cerr << "Exception from command line processing in " << argv[0] << ": no compare file given.\n"
              << "For usage and an options list, please do '" << argv[0] << " --help"
              << "'.\n";
    return process_exit_code::INVALID_ARGUMENT | 2;
  }

  auto source_name = vm["source"].as<std::string>();
  auto compare_name = vm["compare"].as<std::string>();
  auto test_name = std::string{"migratev1v2"};

  auto source_buffer = std::string{};

  if (!db::read_buffer_from_file(source_buffer, source_name)) {
    std::cerr << "Unable to read the source file " << source_name;
    return process_exit_code::INVALID_ARGUMENT | 1;
  }

  auto compare_buffer = std::string{};

  if (!db::read_buffer_from_file(compare_buffer, compare_name)) {
    std::cerr << "Unable to read the compare file " << compare_name;
    return process_exit_code::INVALID_ARGUMENT | 2;
  }

  auto runTest = [](std::string const& name) {
    auto tests = std::map<std::string, test_case>{{"migratev1v2", test_migratev1v2}};

    std::cout << "Running test:<" << name << ">\n";

    return tests.at(name);
  };

  auto testResult = runTest(test_name)(source_buffer, compare_buffer);

  return static_cast<int>(!testResult);
} catch (...) {
  std::cout << "Process exited with error: " << ::debug::current_exception_diagnostic_information();
  return process_exit_code::UNCAUGHT_EXCEPTION;
}

bool test_migratev1v2(std::string const& source, std::string const& compare) {
  confirm(!source.empty());
  confirm(!compare.empty());

  JSONDocument rdoc = JSONDocumentMigrator{{source}};
  JSONDocument cdoc{compare};

  JSONDocumentBuilder returned{rdoc};
  JSONDocumentBuilder expected{cdoc};

  using namespace artdaq::database::overlay;

  auto result = returned == expected;

  if (result.first) {
    return true;
  }

  std::cout << "Error returned!=expected.\n";
  std::cerr << "returned:\n" << returned << "\n";
  std::cerr << "expected:\n" << expected << "\n";
  std::cerr << "error:\n" << result.second << "\n";

  return false;
}
