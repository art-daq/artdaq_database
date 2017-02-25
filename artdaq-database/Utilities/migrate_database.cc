#include "test/common.h"

#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/ConfigurationDB/dboperation_metadata.h"
#include "artdaq-database/JsonDocument/JSONDocument.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"


namespace bpo = boost::program_options;
using namespace artdaq::database;

int main(int argc, char* argv[]) try {
  std::ostringstream descstr;
  descstr << argv[0] << " <-uri <database uri>>";

  bpo::options_description desc = descstr.str();

  desc.add_options()
  ("uri,u", bpo::value<std::string>(),"Database URI")
  ("help,h", "produce help message");

  bpo::variables_map vm;

  try {
    bpo::store(bpo::command_line_parser(argc, argv).options(desc).run(), vm);
    bpo::notify(vm);
  } catch (bpo::error const& e) {
    std::cerr << "Exception from command line processing in " << argv[0] << ": " << e.what() << "\n";
    return process_exit_code::INVALID_ARGUMENT;
  }

  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return process_exit_code::HELP;
  }

  if (!vm.count("uri")) {
    std::cerr << "Exception from command line processing in " << argv[0] << ": no databse URI given.\n"
              << "For usage and an options list, please do '" << argv[0] << " --help"
              << "'.\n";
    return process_exit_code::INVALID_ARGUMENT | 1;
  }

  auto database_uri = vm["uri"].as<std::string>();


  return process_exit_code::SUCCESS;
} catch (...) {
  std::cerr << "Process exited with error: " << ::debug::current_exception_diagnostic_information();
  return process_exit_code::UNCAUGHT_EXCEPTION;
}
