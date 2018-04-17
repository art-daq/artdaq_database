#include "artdaq-database/DataFormats/Json/json_common.h"
#include "test/common.h"

namespace bpo = boost::program_options;
using namespace artdaq::database;
namespace jsn = artdaq::database::json;
namespace literal = artdaq::database::dataformats::literal;

bool migrate_to_string_value(jsn::value_t& value) {
  switch (jsn::type(value)) {
    case jsn::type_t::ARRAY: {
      auto& data = boost::get<jsn::array_t>(value);
      for (auto& element : data) {
        if (!migrate_to_string_value(element)) {
          return false;
        }
      }
    } break;
    case jsn::type_t::OBJECT: {
      auto& data = boost::get<jsn::object_t>(value);
      for (auto& element : data) {
        if (!migrate_to_string_value(element.value)) {
          return false;
        }
      }
    } break;
    case jsn::type_t::VALUE:
      value = boost::apply_visitor(jsn::tostring_visitor(), value);
      break;
    default:
      return false;
  }
  return true;
}

int main(int argc, char* argv[]) try {
  // Get the input parameters via the boost::program_options library,
  // designed to make it relatively simple to define arguments and
  // issue errors if argument list is supplied incorrectly

  std::ostringstream descstr;
  descstr << argv[0] << " <-c <config-file>> <other-options>";

  bpo::options_description desc = descstr.str();

  desc.add_options()("config,c", bpo::value<std::string>(), "Configuration file.")(
      "outputformat,f", bpo::value<std::string>(), "Output file format.")("help,h", "produce help message");

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

  jsn::object_t doc_ast;

  if (!jsn::JsonReader{}.read(json, doc_ast)) {
    confirm(false);
  }

  auto get_SubNode = [](auto& parent, auto const& child_name) -> auto& {
    return boost::get<jsn::object_t>(parent.at(child_name));
  };

  auto& document = get_SubNode(doc_ast, literal::document);

  if (!migrate_to_string_value(document.at(literal::data))) {
    return process_exit_code::FAILURE;
  }

  auto json_converted = std::string{};

  if (!jsn::JsonWriter{}.write(doc_ast, json_converted)) {
    confirm(false);
  }

  if (!db::write_buffer_to_file(json_converted, file_name)) {
    confirm(false);
  }

  return process_exit_code::SUCCESS;
} catch (...) {
  std::cerr << "Process exited with error: " << ::debug::current_exception_diagnostic_information();
  return process_exit_code::UNCAUGHT_EXCEPTION;
}
