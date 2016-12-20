#include <fstream>
#include <iostream>

#include <cstddef>
#include <iterator>
#include <regex>
#include "boost/program_options.hpp"
#include "cetlib/includer.h"
#include "fhiclcpp/extended_value.h"
#include "fhiclcpp/intermediate_table.h"
#include "fhiclcpp/parse.h"
#include "fhiclcpp/parse_shims.h"

using namespace fhicl;
namespace bpo = boost::program_options;

std::ostream& operator<<(std::ostream& out, fhicl::extended_value const& value) {
  out << value.to_string();
  return out;
}

int main(int argc, char* argv[]) try {
  // Get the input parameters via the boost::program_options library,
  // designed to make it relatively simple to define arguments and
  // issue errors if argument list is supplied incorrectly

  std::ostringstream descstr;
  descstr << argv[0] << " <-c <config-file>> <other-options>";

  bpo::options_description desc = descstr.str();

  desc.add_options()("config,c", bpo::value<std::string>(), "Configuration file.")("help,h", "produce help message");

  bpo::variables_map vm;

  try {
    bpo::store(bpo::command_line_parser(argc, argv).options(desc).run(), vm);
    bpo::notify(vm);
  } catch (bpo::error const& e) {
    std::cerr << "Exception from command line processing in " << argv[0] << ": " << e.what() << "\n";
    return -1;
  }

  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return 1;
  }
  if (!vm.count("config")) {
    std::cerr << "Exception from command line processing in " << argv[0] << ": no configuration file given.\n"
              << "For usage and an options list, please do '" << argv[0] << " --help"
              << "'.\n";
    return 2;
  }

  // Check the directories defined by the FHICL_FILE_PATH
  // environmental variable for the *.fcl file whose name was passed to
  // the command line. If not defined, look in the current directory.

  if (getenv("FHICL_FILE_PATH") == nullptr) {
    std::cerr << "INFO: environment variable FHICL_FILE_PATH was not set. "
                 "Using \".\"\n";
    setenv("FHICL_FILE_PATH", ".", 0);
  }

  auto file_name = vm["config"].as<std::string>();

  std::ifstream is(file_name);
  std::string conf((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
  is.close();

  auto idx = std::size_t{0};

  conf.reserve(conf.size() + 512);

  std::for_each(std::sregex_iterator(conf.begin(), conf.end(), std::regex{"(#include\\s)([^'\"]*)"}), std::sregex_iterator(),
                [&conf, &idx](auto& m) { conf.replace(m.position(), m.length(), "fhicl_pound_include_" + std::to_string(idx++) + ":"); });

  std::cout << "input\n" << conf << "\n";

  ::fhicl::intermediate_table fhicl_table;

  ::shims::isSnippetMode(true);

  parse_document(conf, fhicl_table);

  for (auto const& atom : fhicl_table) std::cout << "\n" << atom.first << ":" << atom.second;

  std::cout << "\n";
  return 0;
}

catch (std::string& x) {
  std::cerr << "Exception (type string) caught in driver: " << x << "\n";
  return 1;
}

catch (char const* m) {
  std::cerr << "Exception (type char const*) caught in driver: " << std::endl;
  if (m) {
    std::cerr << m;
  } else {
    std::cerr << "[the value was a null pointer, so no message is available]";
  }
  std::cerr << '\n';
}
