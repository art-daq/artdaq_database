#include "test/common.h"

#include "artdaq-database/DataFormats/Json/json_common.h"

namespace bpo = boost::program_options;
using namespace artdaq::database;

int main(int argc, char* argv[]) try {
  std::ostringstream descstr;
  descstr << argv[0] << " <-c <config-file>> <other-options>";

  bpo::options_description desc = descstr.str();

  desc.add_options()("config,c", bpo::value<std::string>(), "Configuration file.")("outputformat,f", bpo::value<std::string>(),
                                                                                   "Output file format.")("help,h", "produce help message");

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

  if (vm.count("config") == 0u) {
    std::cerr << "Exception from command line processing in " << argv[0] << ": no configuration file given.\n"
              << "For usage and an options list, please do '" << argv[0] << " --help"
              << "'.\n";
    return process_exit_code::INVALID_ARGUMENT | 1;
  }

  auto file_name = vm["config"].as<std::string>();

  std::ifstream is(file_name);

  std::stringstream json_buffer;
  json_buffer << is.rdbuf();
  is.close();

  auto json = json_buffer.str();

  namespace jsn = artdaq::database::json;

  auto reader = jsn::JsonReader{};

  jsn::object_t doc_ast1;

  if (!reader.read(json, doc_ast1)) {
    std::cout << "Failed reading JSON buffer: << " << json_buffer.str() << ">>\n";

    return process_exit_code::FAILURE;
  }

  auto writer = jsn::JsonWriter{};

  auto jsonout = std::string();

  if (!writer.write(doc_ast1, jsonout)) {
    return process_exit_code::FAILURE;
  }
  std::cout << "Initial:" << json << "\n";
  std::cout << "Converted:" << jsonout << "\n";

  jsn::object_t doc_ast2;

  if (!reader.read(jsonout, doc_ast2)) {
    std::cout << "Failed reading JSON buffer: << " << jsonout << ">>\n";

    return process_exit_code::FAILURE;
  }

  auto compare_result = doc_ast1 == doc_ast2;

  if (compare_result.first) {
    std::cout << "Returned:" << jsonout << "\n";
    return process_exit_code::SUCCESS;
  } else {
    std::cout << "Test failed (expected!=returned); error message: " << compare_result.second << "\n";

    std::cout << "Expected:" << json << "\n";
    std::cout << "Returned:" << jsonout << "\n";

    return process_exit_code::FAILURE;
  }
} catch (...) {
  std::cerr << "Process exited with error: " << ::debug::current_exception_diagnostic_information();
  return process_exit_code::UNCAUGHT_EXCEPTION;
}
