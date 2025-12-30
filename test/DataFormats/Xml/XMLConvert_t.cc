
#include "test/common.h"

#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/DataFormats/Json/json_common.h"
#include "artdaq-database/DataFormats/Xml/convertxml2json.h"
#include "artdaq-database/DataFormats/Xml/xml_common.h"
#include "artdaq-database/DataFormats/Xml/xmljsondb.h"

#include <fstream>
#include <regex>

namespace bpo = boost::program_options;
using namespace artdaq::database;

using test_case = bool (*)(const std::string&, const std::string&, const std::string&);

bool test_convertxml2json(std::string const& /*input*/, std::string const& /*compare*/, std::string const& /*source_filename*/);
bool test_convertjson2xml(std::string const& /*input*/, std::string const& /*compare*/, std::string const& /*source_filename*/);

// Helper function to extract test number from filename
// e.g., "test001.src.xml" -> "001"
std::string extract_test_number(const std::string& filename) {
  std::regex pattern(R"(test(\d{3}))");
  std::smatch matches;

  if (std::regex_search(filename, matches, pattern)) {
    return matches[1].str();
  }

  return "unknown";
}

// Helper function to write content to a file
bool write_to_file(const std::string& filename, const std::string& content) {
  std::ofstream outfile(filename);

  if (!outfile.is_open()) {
    std::cerr << "ERROR: Failed to open file for writing: " << filename << "\n";
    return false;
  }

  outfile << content;
  outfile.close();

  std::cout << "Debug file written: " << filename << "\n";
  return true;
}

int main(int argc, char* argv[]) {
  artdaq::database::xml::debug::XmlReader();
  artdaq::database::xml::debug::XmlWriter();
  artdaq::database::xmljson::debug::Xml2Json();
  artdaq::database::xmljson::debug::XmlJson();

  debug::registerUngracefullExitHandlers();
  artdaq::database::useFakeTime(true);

  std::ostringstream descstr;
  descstr << argv[0] << " <-s <source-file>> <-c <compare-with-file>> <-t <test-name>> (available test names: xml2json,json2xml)";

  bpo::options_description desc = descstr.str();

  desc.add_options()("source,s", bpo::value<std::string>(), "Input source file.")(
      "compare,c", bpo::value<std::string>(), "Expected result of convertion.")("testname,t", bpo::value<std::string>(), "Test name.")(
      "help,h", "produce help message");

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

  if (vm.count("source") == 0u) {
    std::cerr << "Exception from command line processing in " << argv[0] << ": no source file given.\n"
              << "For usage and an options list, please do '" << argv[0] << " --help"
              << "'.\n";
    return process_exit_code::INVALID_ARGUMENT | 1;
  }

  if (vm.count("compare") == 0u) {
    std::cerr << "Exception from command line processing in " << argv[0] << ": no compare file given.\n"
              << "For usage and an options list, please do '" << argv[0] << " --help"
              << "'.\n";
    return process_exit_code::INVALID_ARGUMENT | 2;
  }

  if (vm.count("testname") == 0u) {
    std::cerr << "Exception from command line processing in " << argv[0] << ": no test name given.\n"
              << "For usage and an options list, please do '" << argv[0] << " --help"
              << "'.\n";
    return process_exit_code::INVALID_ARGUMENT | 3;
  }

  auto input_name = vm["source"].as<std::string>();
  auto compare_name = vm["compare"].as<std::string>();
  auto test_name = vm["testname"].as<std::string>();

  std::ifstream is1(input_name);

  std::string input((std::istreambuf_iterator<char>(is1)), std::istreambuf_iterator<char>());

  std::ifstream is2(compare_name);

  std::string compare((std::istreambuf_iterator<char>(is2)), std::istreambuf_iterator<char>());

  auto runTest = [](std::string const& name) {
    auto tests = std::map<std::string, test_case>{{"xml2json", test_convertxml2json}, {"json2xml", test_convertjson2xml}};

    std::cout << "Running test:<" << name << ">\n";

    return tests.at(name);
  };

  auto testResult = runTest(test_name)(input, compare, input_name);

  return static_cast<int>(!testResult);
}

bool test_convertxml2json(std::string const& input, std::string const& compare, std::string const& source_filename) {
  confirm(!input.empty());
  confirm(!compare.empty());

  auto output = std::string();

  if (!artdaq::database::xmljson::xml_to_json(input, output)) {
    return false;
  }

  auto compare_result = artdaq::database::json::compare_json_objects(output, compare);
  if (compare_result.first) {
    return true;
  } else {
    std::cout << "Convertion failed; error: " << compare_result.second << "\n";
    std::cerr << "output:\n" << output << "\n";
    std::cerr << "expected:\n" << compare << "\n";

    // Write debug files
    std::string test_number = extract_test_number(source_filename);
    write_to_file("test" + test_number + ".output.json", output);
    write_to_file("test" + test_number + ".expected.json", compare);
  }

  return false;
}

bool test_convertjson2xml(std::string const& input, std::string const& compare, std::string const& source_filename) {
  confirm(!input.empty());
  confirm(!compare.empty());

  auto output = std::string();

  if (!artdaq::database::xmljson::json_to_xml(input, output)) {
    return false;
  }

  if (output == compare) {
    return true;
  } else {
    std::cout << "Convertion failed. \n";
    std::cerr << "output:\n" << output << "\n";
    std::cerr << "expected:\n" << compare << "\n";

    // Write debug files
    std::string test_number = extract_test_number(source_filename);
    write_to_file("test" + test_number + ".output.json", output);
    write_to_file("test" + test_number + ".expected.json", compare);
  }

  return false;
}
