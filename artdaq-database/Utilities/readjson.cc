#include "artdaq-database/DataFormats/Json/json_common.h"
#include "artdaq-database/JsonDocument/JSONDocument.h"
#include "test/common.h"

namespace bpo = boost::program_options;
using namespace artdaq::database;

int main(int argc, char* argv[]) try {
  // Get the input parameters via the boost::program_options library,
  // designed to make it relatively simple to define arguments and
  // issue errors if argument list is supplied incorrectly

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

  auto json = std::string{};
  db::read_buffer_from_file(json, file_name);

  auto reader = jsn::JsonReader{};
  auto writer = jsn::JsonWriter{};

  jsn::object_t doc_ast;

  if (!reader.read(json, doc_ast)) {
    return process_exit_code::FAILURE;
  }

  // int a;
  // std::cout << "Continue ?";
  // std::cin>>a;
  // if(a==0)
  //         return process_exit_code::FAILURE;

  auto out = std::string{};
  out.reserve(1000000);
  for (int i = 0; i < 100; i++) {
    out.clear();

    if (!writer.write(doc_ast, out)) {
      return process_exit_code::FAILURE;
    }

    std::cout << (out.size() > 100 ? "+" : "-");
  }

  return process_exit_code::SUCCESS;
} catch (...) {
  std::cerr << "Process exited with error: " << ::debug::current_exception_diagnostic_information();
  return process_exit_code::UNCAUGHT_EXCEPTION;
}
