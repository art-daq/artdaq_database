#include <fstream>
#include <iostream>

#include <boost/any.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/program_options.hpp>
#include <cstddef>
#include <iterator>
#include <regex>

#include "artdaq-database/SharedCommon/fileststem_functions.h"
#include "artdaq-database/SharedCommon/printStackTrace.h"
#include "artdaq-database/SharedCommon/process_exit_codes.h"
#include "cetlib/includer.h"
#include "fhiclcpp/extended_value.h"
#include "fhiclcpp/intermediate_table.h"
#include "fhiclcpp/parse.h"
#include "fhiclcpp/parse_shims.h"

using namespace fhicl;
namespace bpo = boost::program_options;

auto target_path = std::string{"../refactored"};

auto lookup_version = [](auto const& name, auto const& buffer) {
  static constexpr auto version_prefix = "_v";
  static auto subconfig_versionhash_map = std::map<std::string /*subconfig*/, std::vector<std::size_t /*hash*/>>{};

  auto hash = std::hash<std::string>{}(buffer);
  auto& versions = subconfig_versionhash_map[name];
  auto it = std::find(versions.begin(), versions.end(), hash);

  if (it != versions.end()) return std::string{version_prefix}.append(std::to_string(std::distance(versions.begin(), it) + 1));

  versions.emplace_back(hash);

  return std::string{version_prefix}.append(std::to_string(versions.size()));
};

std::ostream& operator<<(std::ostream& out, std::tuple<std::string const&, fhicl::extended_value const&, std::size_t, bool> const& tuple) {
  auto in_prolog = std::get<3>(tuple);
  auto& value = std::get<1>(tuple);

  if (value.in_prolog != in_prolog) {
    return out;
  }

  auto& key = std::get<0>(tuple);
  auto il = std::get<2>(tuple);

  if (value.is_a(::fhicl::TABLE)) {
    out << key << " : "
        << "{\n";
    std::stringstream ss;
    ::fhicl::extended_value::table_t const& tab = value;
    for (auto& entry : tab) {
      ss << std::make_tuple<std::string const&, fhicl::extended_value const&, std::size_t>(entry.first, entry.second, il + 1, in_prolog);
    }

    auto tmp_file = std::string{key}.append(lookup_version(key, ss.str())).append(".fcl");

    std::ofstream os(std::string{target_path}.append("/").append(tmp_file));
    os << ss.str();
    os.close();
    out << "#include \"" << tmp_file << "\"\n";
    out << "}\n";

  } else if (value.is_a(::fhicl::SEQUENCE)) {
    out << "\n"
        << std::string(il, ' ') << key << ":"
        << " [ ";
    ::fhicl::extended_value::sequence_t const& seq = value;
    for (auto& ev : seq) {
      auto tmp = std::make_tuple<std::string const&, fhicl::extended_value const&, std::size_t>(key + "seq", ev, il + 1, in_prolog);
      out << tmp << " ";
    }
    out << std::string(il, ' ') << "]\n";
  } else {
    out << key << ":" << value.raw_value << (il > 0 ? "\n" : "");
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

  // clang-format off
  desc.add_options()
     ("config,c", bpo::value<std::string>(), "Configuration file.")
     ("help,h", "Produce help message.")
     ("prune,p", "Prune @nil values.")
     ("unresolved,u", "Do not fully resolve.")
     ("showprolog,s", "Show FHiCL prolog.");
  // clang-format on
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
  bool fully_resolve = true;
  bool show_prolog = false;

  if (vm.count("prune") != 0u) {
    prune_intermediate_table = true;
  }

  if (vm.count("unresolved") != 0u) {
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

    std::for_each(std::sregex_iterator(conf.begin(), conf.end(), regex), std::sregex_iterator(),
                  [&conf, &idx](auto& m) { conf.replace(m.position(), m.length(), "fhicl_pound_include_" + std::to_string(idx++) + ":"); });
  }

  ::fhicl::intermediate_table fhicl_table;

  if (!fully_resolve) {
    ::shims::isSnippetMode(true);
  }

  parse_document(conf, fhicl_table);

  if (!artdaq::database::mkdir(target_path)) {
    std::cerr << "ERROR: unable to create the \"" << target_path << "\" directory.";
    return 3;
  }

  std::ofstream os(std::string{target_path}.append("/").append(file_name));

  if (show_prolog) {
    std::cout << "BEGIN_PROLOG\n";
    for (auto const& atom : fhicl_table) {
      if (!atom.second.in_prolog) {
        continue;
      }
      if (atom.second.to_string().empty()) {
        if (!prune_intermediate_table) {
          os << atom.first << ":"
             << "@nil\n";
        }
      } else {
        os << std::make_tuple<std::string const&, fhicl::extended_value const&, std::size_t, bool>(atom.first, atom.second, 0, true) << "\n";
      }
    }
    os << "END_PROLOG\n\n";
  }

  for (auto const& atom : fhicl_table) {
    if (atom.second.in_prolog) {
      continue;
    }
    if (atom.second.to_string().empty()) {
      if (!prune_intermediate_table) {
        os << atom.first << ":"
           << "@nil\n";
      }
    } else {
      os << std::make_tuple<std::string const&, fhicl::extended_value const&, std::size_t, bool>(atom.first, atom.second, 0, false) << "\n";
    }
  }

  os << "\n";
  os.close();
  return 0;
} catch (...) {
  std::cerr << "Process exited with error: " << ::debug::current_exception_diagnostic_information();
  return process_exit_code::UNCAUGHT_EXCEPTION;
}

