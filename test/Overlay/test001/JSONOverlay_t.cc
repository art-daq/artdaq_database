#include "test/common.h"

#include "artdaq-database/Overlay/JSONDocumentOverlay.h"

namespace bpo = boost::program_options;

namespace ovl = artdaq::database::overlay;

using artdaq::database::json::JsonReader;

using artdaq::database::json::object_t;
using artdaq::database::json::value_t;

int main(int argc, char* argv[]) try {
  debug::registerUngracefullExitHandlers();

  std::ostringstream descstr;
  descstr << argv[0] << " <-s <config-file>> <other-options>";

  bpo::options_description desc = descstr.str();

  desc.add_options()("source,s", bpo::value<std::string>(), "Source file.")(
      "result,x", bpo::value<std::string>(), "Result file.")("mask,m", bpo::value<uint32_t>(), "Compare mask file.")(
      "fail,f", "Test should fail.")("help,h", "produce help message");

  bpo::variables_map vm;

  try {
    bpo::store(bpo::command_line_parser(argc, argv).options(desc).run(), vm);
    bpo::notify(vm);
  } catch (bpo::error const& e) {
    std::cerr << "Exception from command line processing in " << argv[0] << ": " << e.what() << "\n";
    return process_exit_code::INVALID_ARGUMENT;
  }

  if (vm.count("result") == 0u) {
    std::cerr << "Exception from command line processing in " << argv[0] << ": no result configuration file given.\n"
              << "For usage and an options list, please do '" << argv[0] << " --help"
              << "'.\n";
    return process_exit_code::INVALID_ARGUMENT | 1;
  }

  if (vm.count("source") == 0u) {
    std::cerr << "Exception from command line processing in " << argv[0] << ": no source configuration file given.\n"
              << "For usage and an options list, please do '" << argv[0] << " --help"
              << "'.\n";
    return process_exit_code::INVALID_ARGUMENT | 1;
  }

  auto mask = uint32_t{0};

  auto should_succeed = bool{true};

  if (vm.count("mask") != 0u) {
    mask = vm["mask"].as<uint32_t>();
  }

  if (vm.count("fail") != 0u) {
    should_succeed = false;
  }

  auto file_src = vm["source"].as<std::string>();
  std::ifstream is1(file_src);
  std::string conf1((std::istreambuf_iterator<char>(is1)), std::istreambuf_iterator<char>());

  auto file_res = vm["result"].as<std::string>();
  std::ifstream is2(file_res);
  std::string conf2((std::istreambuf_iterator<char>(is2)), std::istreambuf_iterator<char>());

  ovl::useCompareMask(mask);

  auto ast1 = object_t{};
  auto ast2 = object_t{};

  bool testResult1 = JsonReader().read(conf1, ast1);
  bool testResult2 = JsonReader().read(conf2, ast2);

  if (!testResult1 || !testResult2) {
    return process_exit_code::FAILURE;
  }

  value_t value1 = ast1;
  value_t value2 = ast2;

  ovl::ovlDatabaseRecord record1(value1);
  // std::cout << "record1 =<" << record1.to_string() << ">" ;

  ovl::ovlDatabaseRecord record2(value2);
  // std::cout << "record2 =<" << record2.to_string() << ">" ;

  auto result = record1 == record2;

  if (result.first && should_succeed) {
    return process_exit_code::SUCCESS;
  } else if (!result.first && !should_succeed) {
    std::cout << "Compare mask=" << mask << "\n";
    std::cout << "Result:" << result.second << "\n";
    return process_exit_code::SUCCESS;
  }

  std::cout << "Compare mask=" << std::bitset<32>(ovl::useCompareMask()) << "\n";
  std::cout << "Result:" << result.second << "\n";

  return process_exit_code::FAILURE;
} catch (...) {
  std::cerr << "Process exited with error: " << ::debug::current_exception_diagnostic_information();
  return process_exit_code::UNCAUGHT_EXCEPTION;
}
