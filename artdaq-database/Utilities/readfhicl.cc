#include <fstream>
#include <iostream>

#include <boost/any.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/program_options.hpp>
#include <cstddef>
#include <iterator>
#include <regex>

#include "artdaq-database/SharedCommon/printStackTrace.h"
#include "artdaq-database/SharedCommon/process_exit_codes.h"

#include "cetlib/includer.h"
#include "fhiclcpp/extended_value.h"
#include "fhiclcpp/intermediate_table.h"
#include "fhiclcpp/parse.h"
#include "fhiclcpp/parse_shims.h"

using namespace fhicl;
namespace bpo = boost::program_options;

std::ostream& operator<<(std::ostream& out, std::tuple<fhicl::extended_value const&, std::size_t, bool> const& tuple) {
  auto& value = std::get<0>(tuple);
  auto il = std::get<1>(tuple);
  auto in_prolog = std::get<2>(tuple);

  if (value.in_prolog != in_prolog) {
    return out;
  }

  if (value.is_a(::fhicl::TABLE)) {
    out << "\n" << std::string(il, ' ') << "{\n";
    ::fhicl::extended_value::table_t const& tab = value;
    for (auto& entry : tab) {
      out << std::string(il + 1, ' ') << entry.first << ":";
      auto tmp = std::make_tuple<fhicl::extended_value const&, std::size_t>(entry.second, il + 1, in_prolog);
      out << tmp;
    }
    out << std::string(il, ' ') << "}\n";
  } else if (value.is_a(::fhicl::SEQUENCE)) {
    out << "\n" << std::string(il, ' ') << "[ ";
    ::fhicl::extended_value::sequence_t const& seq = value;
    for (auto& ev : seq) {
      auto tmp = std::make_tuple<fhicl::extended_value const&, std::size_t>(ev, il + 1, in_prolog);
      out << tmp << " ";
    }
    out << std::string(il, ' ') << "]\n";
  } else {
    out << value.raw_value << (il > 0 ? "\n" : "");
  }
  return out;
}

int main(int argc, char* argv[]) try {
  // Get the input parameters via the boost::program_options library,
  // designed to make it relatively simple to define arguments and
  // issue errors if argument list is supplied incorrectly

  std::ostringstream descstr;
  descstr << argv[0] << " <-c <config-file>> <other-options>";

  bpo::options_description desc = descstr.str();

  desc.add_options()("config,c", bpo::value<std::string>(), "Configuration file.")("help,h", "Produce help message.")(
      "prune,p", "Prune @nil values.")("resolve,r", "Fully resolve.")("showprolog,s", "Show FHiCL prolog.");

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

  bool prune_intermediate_table = false;
  bool fully_resolve = false;
  bool show_prolog = false;

  if (vm.count("prune") != 0u) {
    prune_intermediate_table = true;
  }

  if (vm.count("resolve") != 0u) {
    fully_resolve = true;
  }

  if (vm.count("showprolog") != 0u) {
    show_prolog = true;
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

  std::ifstream is(file_name);
  std::string conf((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
  is.close();

  if (!fully_resolve) {
    auto idx = std::size_t{0};

    conf.reserve(conf.size() + 512);

    auto regex = std::regex{"(#include\\s)([^'\"]*)"};

    std::for_each(std::sregex_iterator(conf.begin(), conf.end(), regex), std::sregex_iterator(), [&conf, &idx](auto& m) {
      conf.replace(m.position(), m.length(), "fhicl_pound_include_" + std::to_string(idx++) + ":");
    });
  }

  ::fhicl::intermediate_table fhicl_table;

  if (!fully_resolve) {
    ::shims::isSnippetMode(true);
  }

  parse_document(conf, fhicl_table);

  if (show_prolog) {
    std::cout << "BEGIN_PROLOG\n";
    for (auto const& atom : fhicl_table) {
      if (!atom.second.in_prolog) {
        continue;
      }
      if (atom.second.to_string().empty()) {
        if (!prune_intermediate_table) {
          std::cout << atom.first << ":"
                    << "@nil\n";
        }
      } else {
        auto tmp = std::make_tuple<fhicl::extended_value const&, std::size_t, bool>(atom.second, 0, true);
        std::cout << atom.first << ":" << tmp << "\n";
      }
    }
    std::cout << "END_PROLOG\n\n";
  }

  for (auto const& atom : fhicl_table) {
    if (atom.second.in_prolog) {
      continue;
    }
    if (atom.second.to_string().empty()) {
      if (!prune_intermediate_table) {
        std::cout << atom.first << ":"
                  << "@nil\n";
      }
    } else {
      auto tmp = std::make_tuple<fhicl::extended_value const&, std::size_t, bool>(atom.second, 0, false);
      std::cout << atom.first << ":" << tmp << "\n";
    }
  }

  std::cout << "\n";
  return 0;
} catch (...) {
  std::cerr << "Process exited with error: " << ::debug::current_exception_diagnostic_information();
  return process_exit_code::UNCAUGHT_EXCEPTION;
}
