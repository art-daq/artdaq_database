#include <fstream>
#include <iostream>
#include <streambuf>
#include <string>
#include "boost/program_options.hpp"

#include "artdaq-database/FhiclJson/convertjson2guijson.h"

#include <boost/exception/diagnostic_information.hpp>
#include "artdaq-database/BuildInfo/process_exit_codes.h"

namespace bpo = boost::program_options;
using namespace artdaq::database;

int main(int argc, char* argv[]) try {
  // Get the input parameters via the boost::program_options library,
  // designed to make it relatively simple to define arguments and
  // issue errors if argument list is supplied incorrectly

  std::ostringstream descstr;
  descstr << argv[0] << " <-i <input-file>> <-o <output-file>> <-f <format>>";

  bpo::options_description desc = descstr.str();

  desc.add_options()("input,i", bpo::value<std::string>(), "Input JSON file.")("output,o", bpo::value<std::string>(),
                                                                               "Output JSON file.")(
      "format,f", bpo::value<std::string>(), "JSON format db or gui (default).")("help,h", "produce help message");

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

  if (!vm.count("input")) {
    std::cerr << "Exception from command line processing in " << argv[0] << ": no input file name given.\n"
              << "For usage and an options list, please do '" << argv[0] << " --help"
              << "'.\n";
    return process_exit_code::INVALID_ARGUMENT | 1;
  }

  if (!vm.count("output")) {
    std::cerr << "Exception from command line processing in " << argv[0] << ": no output file given.\n"
              << "For usage and an options list, please do '" << argv[0] << " --help"
              << "'.\n";
    return process_exit_code::INVALID_ARGUMENT | 2;
  }

  auto format = std::string("gui");

  if (!vm.count("format")) {
    std::cout << "Warn: Converting to " << format << " format.\n";
  } else {
    format = vm["format"].as<std::string>();
  }

  auto input_file_name = vm["input"].as<std::string>();
  auto output_file_name = vm["output"].as<std::string>();

  std::ifstream is(input_file_name);

  std::stringstream input_buffer;
  input_buffer << is.rdbuf();
  is.close();

  auto input_json = input_buffer.str();

  auto output_json = std::string();

  auto result = bool{false};

  if (format.compare("db") == 0 || format.compare("DB") == 0) {
    result = artdaq::database::json_db_to_gui(input_json, output_json);
  } else {
    result = artdaq::database::json_gui_to_db(input_json, output_json);
  }

  std::ofstream os(output_file_name);
  os << output_json;
  os.close();

  std::cout << output_json << "\n";

  if (result) return process_exit_code::SUCCESS;

  return process_exit_code::FAILURE;
} catch (...) {
  std::cerr << "Process exited with error: " << boost::current_exception_diagnostic_information();
  return process_exit_code::UNCAUGHT_EXCEPTION;
}
