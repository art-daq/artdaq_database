#include <boost/exception/diagnostic_information.hpp>
#include <fstream>
#include <iostream>
#include <streambuf>
#include <string>
#include "artdaq-database/BuildInfo/printStackTrace.h"
#include "artdaq-database/BuildInfo/process_exit_codes.h"
#include "artdaq-database/ConfigurationDB/configurationdb.h"
#include "boost/program_options.hpp"
#include "cetlib/coded_exception.h"

#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/FhiclJson/fhicljsondb.h"
#include "artdaq-database/JsonDocument/JSONDocument.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h"
#include "artdaq-database/StorageProviders/MongoDB/provider_mongodb.h"

namespace bpo = boost::program_options;

namespace db = artdaq::database;
namespace cf = db::configuration;
namespace cfo = cf::options;
namespace cfol = cfo::literal;

using Options = cfo::FindConfigsOperation;

using artdaq::database::jsonutils::JSONDocument;
using artdaq::database::basictypes::JsonData;

typedef bool (*test_case)(Options const& /*options*/, std::string const& /*file_name*/);

bool test_findconfigs(Options const&, std::string const&);
bool test_buildfilter(Options const&, std::string const&);

int main(int argc, char* argv[]) try {
  debug::registerUngracefullExitHandlers();
  //    artdaq::database::fhicljson::useFakeTime(true);

  cf::trace_enable_FindConfigsOperation();
  cf::trace_enable_FindConfigsOperationDetail();
  cf::trace_enable_DBOperationMongo();
  cf::trace_enable_DBOperationFileSystem();

  db::filesystem::trace_enable();
  db::mongo::trace_enable();

  db::fhicljson::trace_enable_fcl2jsondb();
  db::fhicl::trace_enable_FhiclReader();
  db::fhicl::trace_enable_FhiclWriter();
  db::fhicljsondb::trace_enable_fhicljsondb();

  db::jsonutils::trace_enable_JSONDocumentBuilder();
  db::jsonutils::trace_enable_JSONDocument();

  // Get the input parameters via the boost::program_options library,
  // designed to make it relatively simple to define arguments and
  // issue errors if argument list is supplied incorrectly

  namespace db = artdaq::database;
  namespace cfol = artdaq::database::configuration::options::literal;
  std::ostringstream descstr;
  descstr << argv[0] << " <-o <operation>>  <-d <database>> <-c <compare-file>>  <-g <globalid>> ";

  bpo::options_description desc = descstr.str();

  desc.add_options()("operation,o", bpo::value<std::string>(), "Operation [findconfigs/buildfilter].")(
      "database,d", bpo::value<std::string>(), "Database provider name.")("compare,c", bpo::value<std::string>(),
                                                                          "Expected result file name.")(
      "globalid,g", bpo::value<std::string>(), "Gloval config id.")("help,h", "produce help message");

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
  auto options = Options{};

  if (!vm.count("operation")) {
    std::cerr << "Exception from command line processing in " << argv[0] << ": no database operation given.\n"
              << "For usage and an options list, please do '" << argv[0] << " --help"
              << "'.\n";
    return process_exit_code::INVALID_ARGUMENT | 1;
  } else {
    options.operation(vm["operation"].as<std::string>());
  }

  if (!vm.count("database")) {
    std::cerr << "Exception from command line processing in " << argv[0] << ": no datbase provider name given.\n"
              << "For usage and an options list, please do '" << argv[0] << " --help"
              << "'.\n";
    return process_exit_code::INVALID_ARGUMENT | 2;
  } else {
    options.provider(vm["database"].as<std::string>());
  }

  if (!vm.count("compare")) {
    std::cerr << "Exception from command line processing in " << argv[0] << ": no compare file name given.\n"
              << "For usage and an options list, please do '" << argv[0] << " --help"
              << "'.\n";
    return process_exit_code::INVALID_ARGUMENT | 3;
  }

  if (vm.count("globalid")) {
    options.globalConfigurationId(vm["globalid"].as<std::string>());
  }

  options.dataFormat("gui");

  auto file_name = vm["compare"].as<std::string>();

  std::cout << "Running test:<" << options.operation() << ">\n";

  auto runTest = [](std::string const& name) {
    auto tests = std::map<std::string, test_case>{{"findconfigs", test_findconfigs}, {"buildfilter", test_buildfilter}};

    return tests.at(name);
  };

  auto testResult = runTest(options.operation())(options, file_name);

  if (testResult) return process_exit_code::SUCCESS;

  return process_exit_code::FAILURE;
} catch (...) {
  std::cerr << "Process exited with error: " << boost::current_exception_diagnostic_information();
  return process_exit_code::UNCAUGHT_EXCEPTION;
}

bool test_findconfigs(Options const& options, std::string const& file_name) {
  assert(!file_name.empty());

  std::ifstream is(file_name);

  std::string expected_json((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());

  is.close();

  auto result = cf::find_global_configurations(options);

  if (!result.first) {
    std::cerr << "Error message: " << result.second << "\n";
    return false;
  }

  auto expected = JSONDocument(expected_json);
  auto returned = JSONDocument(result.second);

  if (returned == expected) {
    return true;
  } else {
    std::cout << "Search failed. \n";
    std::cerr << "returned:\n" << returned << "\n";
    std::cerr << "expected:\n" << expected << "\n";
  }

  return false;
}

bool test_buildfilter(Options const& options, std::string const& file_name) {
  assert(!file_name.empty());

  std::ifstream is(file_name);

  std::string expected_json((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());

  is.close();

  auto result = cf::build_global_configuration_search_filter(options);

  if (!result.first) {
    std::cerr << "Error message: " << result.second << "\n";
    return false;
  }

  auto expected = JSONDocument(expected_json);
  auto returned = JSONDocument(result.second);

  if (returned == expected) {
    return true;
  } else {
    std::cout << "Search failed. \n";
    std::cerr << "returned:\n" << returned << "\n";
    std::cerr << "expected:\n" << expected << "\n";
  }

  return false;
}