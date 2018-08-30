#include <fstream>
#include <iostream>

#include <boost/any.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/program_options.hpp>
#include <cstddef>
#include <iterator>
#include <regex>

#include "artdaq-database/ConfigurationDB/configurationdb.h"
#include "artdaq-database/DataFormats/Fhicl/fhicljsondb.h"
#include "artdaq-database/DataFormats/Json/json_common.h"
#include "artdaq-database/JsonDocument/JSONDocument.h"
#include "artdaq-database/SharedCommon/printStackTrace.h"
#include "artdaq-database/SharedCommon/process_exit_codes.h"

namespace bpo = boost::program_options;
namespace impl = artdaq::database::configuration::json;
namespace fjlib = artdaq::database::fhicljson;

using result_t = std::pair<bool, std::string>;

result_t fhicl_to_json(std::string const& fcl, std::string const& filename);
result_t json_to_fhicl(std::string const& jsn, std::string& filename);

int main(int argc, char* argv[]) try {
  // impl::enable_trace();

  std::ostringstream descstr;
  descstr << argv[0] << " <-c <config-file>> <other-options>";

  bpo::options_description desc = descstr.str();

  desc.add_options()("config,c", bpo::value<std::string>(), "Configuration file.")(
      "main,m", "Output the \"document.data.main\" subtree.")("help,h", "Produce help message.");

  bpo::variables_map vm;

  try {
    bpo::store(bpo::command_line_parser(argc, argv).options(desc).run(), vm);
    bpo::notify(vm);
  } catch (bpo::error const& e) {
    std::cerr << "Exception from command line processing in " << argv[0] << ": " << e.what() << "\n";
    return -1;
  }

  if (vm.count("help") != 0u) {
    std::cout << desc << std::endl;
    return 1;
  }

  if (vm.count("config") == 0u) {
    std::cerr << "Exception from command line processing in " << argv[0] << ": no configuration file given.\n"
              << "For usage and an options list, please do '" << argv[0] << " --help"
              << "'.\n";
    return 2;
  }

  // Check the directories defined by the FHICL_FILE_PATH
  // environmental variable for the *.fcl file whose name was passed to
  // the command line. If not defined, look in the current directory.

  if (getenv("FHICL_FILE_PATH") == nullptr) {
    std::cerr << "INFO: environment variable FHICL_FILE_PATH was not set. Using \".\"\n";
    setenv("FHICL_FILE_PATH", ".", 0);
  }

  auto file_name = vm["config"].as<std::string>();

  auto main = bool{false};

  if (vm.count("main") != 0u) {
    main = true;
  }

  std::ifstream is(file_name);
  std::string fhicl_buffer((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
  is.close();

  auto json_buffer = std::string{};

  auto result = fhicl_to_json(fhicl_buffer, file_name);

  if (!result.first) {
    std::cerr << "Error: Convertion failed, error message: " << result.second << "\n";
    return process_exit_code::FAILURE;
  }

  if (main) {
    auto json_subnode = std::string{"document.data.main"};
    result = {true, artdaq::database::docrecord::JSONDocument(result.second).findChild(json_subnode).to_string()};
  }

  std::cout << result.second;

  return process_exit_code::SUCCESS;
} catch (...) {
  std::cerr << "Process exited with error: " << ::debug::current_exception_diagnostic_information();
  return process_exit_code::UNCAUGHT_EXCEPTION;
}

result_t fhicl_to_json(std::string const& fcl, std::string const& filename) {
  artdaq::database::set_default_locale();
  auto retValue = std::string{};
  auto result = fjlib::fhicl_to_json(fcl, filename, retValue);
  return {result, retValue};
}

result_t json_to_fhicl(std::string const& jsn, std::string& filename) {
  artdaq::database::set_default_locale();
  auto retValue = std::string{};
  auto result = fjlib::json_to_fhicl(jsn, retValue, filename);
  return {result, retValue};
}