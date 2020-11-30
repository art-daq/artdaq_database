#include "test/common.h"

#include "artdaq-database/DataFormats/Fhicl/convertfhicl2jsondb.h"
#include "artdaq-database/DataFormats/Fhicl/fhicl_common.h"
#include "artdaq-database/DataFormats/Fhicl/fhicljsondb.h"

#include "artdaq-database/DataFormats/Json/convertjson2guijson.h"
#include "artdaq-database/DataFormats/Json/json_common.h"

namespace bpo = boost::program_options;
using namespace artdaq::database;

using test_case = bool (*)(const std::string&, const std::string&);

bool test_convert2fcl(std::string const& /*input*/, std::string const& /*compare*/);
bool test_convert2json(std::string const& /*input*/, std::string const& /*compare*/);
bool test_roundconvertfcl(std::string const& /*input*/, std::string const& /*compare*/);
bool test_roundconvertjson(std::string const& /*input*/, std::string const& /*compare*/);

int main(int argc, char* argv[]) try {
  artdaq::database::fhicl::debug::FhiclReader();
  artdaq::database::fhicl::debug::FhiclWriter();
  artdaq::database::fhicljson::debug::FhiclJson();
  artdaq::database::fhicljson::debug::FCL2JSON();

  debug::registerUngracefullExitHandlers();
  artdaq::database::useFakeTime(true);

  std::ostringstream descstr;
  descstr << argv[0]
          << " <-s <source-file>> <-c <compare-with-file>> <-t <test-name>> (available test names: "
             "Convert_Fhicl2DBJson,Convert_DBJson2Fhicl, "
             "RoundConvert_Fhicl2DBJson,RoundConvert_DBJson2Fhicl)";

  bpo::options_description desc = descstr.str();

  desc.add_options()("source,s", bpo::value<std::string>(), "Input source file.")(
      "compare,c", bpo::value<std::string>(), "Expected result of convertion.")("testname,t", bpo::value<std::string>(), "Test name.")

      ("help,h", "produce help message");

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
    auto tests = std::map<std::string, test_case>{{"Convert_Fhicl2DBJson", test_convert2json},
                                                  {"Convert_DBJson2Fhicl", test_convert2fcl},
                                                  {"RoundConvert_Fhicl2DBJson", test_roundconvertfcl},
                                                  {"RoundConvert_DBJson2Fhicl", test_roundconvertjson}};

    std::cout << "Running test:<" << name << ">\n";

    return tests.at(name);
  };

  auto testResult = runTest(test_name)(input, compare);

  if (testResult) {
    return process_exit_code::SUCCESS;
  }

  return process_exit_code::FAILURE;
} catch (...) {
  std::cerr << "Process exited with error: " << ::debug::current_exception_diagnostic_information();
  return process_exit_code::UNCAUGHT_EXCEPTION;
}

bool test_convert2fcl(std::string const& input, std::string const& compare) {
  confirm(!input.empty());
  confirm(!compare.empty());

  auto output = std::string();
  auto filename = std::string();

  if (!fhicljson::json_to_fhicl(input, output, filename)) {
    std::cout << "Convertion failed. \n";
    std::cerr << "input:\n" << input << "\n";
    std::cerr << "output:\n" << output << "\n";
    return false;
  };

  if (output == compare) {
    return true;
  } else {
    std::cout << "Convertion failed. \n";
    std::cerr << "output:\n" << output << "\n";
    std::cerr << "expected:\n" << compare << "\n";

    auto mismatch = std::mismatch(compare.begin(), compare.end(), output.begin());
    std::cerr << "File sizes (exp,ret)=(" << std::distance(compare.begin(), compare.end()) << "," << std::distance(output.begin(), output.end())
              << ")\n";

    std::cerr << "First mismatch at position " << std::distance(compare.begin(), mismatch.first) << ", (exp,ret)=(0x" << std::hex
              << static_cast<unsigned int>(*mismatch.first) << ",0x" << static_cast<unsigned int>(*mismatch.second) << ")\n";
  }

  return false;
}

bool test_convert2json(std::string const& input, std::string const& compare) {
  confirm(!input.empty());
  confirm(!compare.empty());

  auto output = std::string();
  auto filename = std::string("notprovided");

  std::cout << "Convertion started. \n";

  if (!fhicljson::fhicl_to_json(input, filename, output)) {
    std::cout << "Convertion failed. \n";
    std::cerr << "input:\n" << input << "\n";
    std::cerr << "output:\n" << output << "\n";
    return false;
  }

  auto compare_result = artdaq::database::json::compare_json_objects(output, compare);
  if (compare_result.first) {
    return true;
  } else {
    std::cout << "Convertion failed; error: " << compare_result.second << "\n";
    std::cerr << "output:\n" << output << "\n";
    std::cerr << "expected:\n" << compare << "\n";
  }

  return false;
}

bool test_roundconvertfcl(std::string const& input, std::string const& compare) {
  confirm(!input.empty());
  confirm(!compare.empty());

  auto tmp = std::string();
  auto output = std::string();
  auto filename = std::string("notprovided");

  try {
    if (!fhicljson::fhicl_to_json(input, filename, tmp)) {
      std::cout << "Convertion failed. \n";
      std::cerr << "input:\n" << input << "\n";
      std::cerr << "tmp:\n" << tmp << "\n";
      return false;
    }

    std::cout << "fhicl_to_json succeeded.\n";

    if (!fhicljson::json_to_fhicl(tmp, output, filename)) {
      std::cout << "Convertion failed. \n";
      std::cerr << "tmp:\n" << tmp << "\n";
      std::cerr << "output:\n" << output << "\n";
      return false;
    };

    std::cout << "json_to_fhicl succeeded.\n";

  } catch (...) {
    std::cout << "Convertion failed. \n";
    std::cerr << "output:\n" << output << "\n";
    std::cerr << "expected:\n" << compare << "\n";
    std::cerr << "tmp:\n" << tmp << "\n";
    throw;
  }

  if (output == compare) {
    return true;
  } else {
    std::cout << "Convertion failed. \n";
    std::cerr << "output:\n" << output << "\n";
    std::cerr << "expected:\n" << compare << "\n";
    std::cerr << "tmp:\n" << tmp << "\n";

    auto mismatch = std::mismatch(compare.begin(), compare.end(), output.begin());
    std::cerr << "File sizes (exp,ret)=(" << std::distance(compare.begin(), compare.end()) << "," << std::distance(output.begin(), output.end())
              << ")\n";

    std::cerr << "First mismatch at position " << std::distance(compare.begin(), mismatch.first) << ", (exp,ret)=(0x" << std::hex
              << static_cast<unsigned int>(*mismatch.first) << ",0x" << static_cast<unsigned int>(*mismatch.second) << ")\n";
  }

  return false;
}

bool test_roundconvertjson(std::string const& input, std::string const& compare) {
  confirm(!input.empty());
  confirm(!compare.empty());

  auto tmp = std::string();
  auto output = std::string();
  auto filename = std::string();

  try {
    if (!fhicljson::json_to_fhicl(input, tmp, filename)) {
      std::cout << "Convertion failed. \n";
      std::cerr << "input:\n" << input << "\n";
      std::cerr << "tmp:\n" << tmp << "\n";
      return false;
    }

    std::cout << "json_to_fhicl succeeded.\n";

    if (!fhicljson::fhicl_to_json(tmp, filename, output)) {
      std::cout << "Convertion failed. \n";
      std::cerr << "tmp:\n" << tmp << "\n";
      std::cerr << "output:\n" << output << "\n";
      return false;
    };

    std::cout << "fhicl_to_json succeeded.\n";

  } catch (...) {
    std::cout << "Convertion failed. \n";
    std::cerr << "output:\n" << output << "\n";
    std::cerr << "expected:\n" << compare << "\n";
    std::cerr << "tmp:\n" << tmp << "\n";
    throw;
  }

  auto compare_result = artdaq::database::json::compare_json_objects(output, compare);
  if (compare_result.first) {
    return true;
  } else {
    std::cout << "Convertion failed. \n";
    std::cerr << "output:\n" << output << "\n";
    std::cerr << "expected:\n" << compare << "\n";
    std::cerr << "tmp:\n" << tmp << "\n";

    auto filename = std::string{"/tmp/test.json"};
    db::write_buffer_to_file(output, filename);
  }

  return false;
}
