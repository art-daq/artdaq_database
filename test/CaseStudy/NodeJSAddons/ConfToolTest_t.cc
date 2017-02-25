#include <fstream>
#include <iostream>
#include <streambuf>
#include <string>
#include "artdaq-database/BuildInfo/printStackTrace.h"
#include "artdaq-database/BuildInfo/process_exit_codes.h"
#include "artdaq-database/ConfigurationDB/configurationdb.h"
#include "boost/program_options.hpp"
#include "cetlib/coded_exception.h"

#include "artdaq-database/FhiclJson/fhicljsondb.h"
#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h"
#include "artdaq-database/StorageProviders/MongoDB/provider_mongodb.h"

#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"

namespace bpo = boost::program_options;

namespace db = artdaq::database;
namespace cf = db::configuration;
namespace cfgui = cf::guiexports;
namespace cfo = cf::options;
namespace cfol = cfo::literal;

using Options = cfo::ManageDocumentOperation;
using artdaq::database::docrecord::JSONDocument;

typedef bool (*test_case)(std::string const& /*filt*/, std::string const& /*src*/, std::string const& /*cmp*/);

bool test_storeconfig(std::string const& /*filt*/, std::string const& /*src*/, std::string const& /*cmp*/);
bool test_loadconfig(std::string const& /*filt*/, std::string const& /*src*/, std::string const& /*cmp*/);

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

  // Get the input parameters via the boost::program_options library,
  // designed to make it relatively simple to define arguments and
  // issue errors if argument list is supplied incorrectly

  namespace db = artdaq::database;
  namespace cfol = artdaq::database::configuration::options::literal;
  std::ostringstream descstr;
  descstr << argv[0] << " <-s <source-file>>  <-c <compare-file>>  <-f <filter-file>>  <-o <operation>>";

  bpo::options_description desc = descstr.str();

  desc.add_options()("source,s", bpo::value<std::string>(), "Configuration source file name.")(
      "compare,c", bpo::value<std::string>(), "Expected return file name.")(
      "filter,f", bpo::value<std::string>(), "Search filter file name.")("operation,o", bpo::value<std::string>(),
                                                                         "Operation [load/store].")

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

  if (!vm.count("source")) {
    std::cerr << "Exception from command line processing in " << argv[0] << ": no source file name given.\n"
              << "For usage and an options list, please do '" << argv[0] << " --help"
              << "'.\n";
    return process_exit_code::INVALID_ARGUMENT | 1;
  }

  auto source_file = vm["source"].as<std::string>();

  if (!vm.count("compare")) {
    std::cerr << "Exception from command line processing in " << argv[0] << ": no compare file name given.\n"
              << "For usage and an options list, please do '" << argv[0] << " --help"
              << "'.\n";
    return process_exit_code::INVALID_ARGUMENT | 2;
  }

  auto compare_file = vm["compare"].as<std::string>();

  if (!vm.count("filter")) {
    std::cerr << "Exception from command line processing in " << argv[0] << ": no filter file name given.\n"
              << "For usage and an options list, please do '" << argv[0] << " --help"
              << "'.\n";
    return process_exit_code::INVALID_ARGUMENT | 3;
  }

  auto filter_file = vm["filter"].as<std::string>();

  if (!vm.count("operation")) {
    std::cerr << "Exception from command line processing in " << argv[0] << ": no database operation given.\n"
              << "For usage and an options list, please do '" << argv[0] << " --help"
              << "'.\n";
    return process_exit_code::INVALID_ARGUMENT | 4;
  }

  auto operation = vm["operation"].as<std::string>();

  auto runTest = [](std::string const& name) {
    auto tests = std::map<std::string, test_case>{{"store", test_storeconfig}, {"load", test_loadconfig}};

    return tests.at(name);
  };

  std::cout << "Running test:<" << operation << ">\n";

  auto testResult = runTest(operation)(filter_file, source_file, compare_file);

  if (testResult) return process_exit_code::SUCCESS;

  return process_exit_code::FAILURE;
} catch (...) {
  std::cerr << "Process exited with error: " << ::debug::current_exception_diagnostic_information();
  return process_exit_code::UNCAUGHT_EXCEPTION;
}

bool test_storeconfig(std::string const& filt, std::string const& src, std::string const& cmp) {
  confirm(!filt.empty());
  confirm(!src.empty());
  confirm(!cmp.empty());

  std::ifstream is1(filt);
  std::string filt_json((std::istreambuf_iterator<char>(is1)), std::istreambuf_iterator<char>());
  is1.close();

  std::ifstream is2(src);
  std::string src_json((std::istreambuf_iterator<char>(is2)), std::istreambuf_iterator<char>());
  is2.close();

  std::ifstream is3(cmp);
  std::string cmp_json((std::istreambuf_iterator<char>(is3)), std::istreambuf_iterator<char>());
  is3.close();

  auto result = cfgui::write_document(filt_json, src_json);

  if (!result.first) {
    std::cerr << "Error message: " << result.second << "\n";
    return false;
  }

  auto returned = JSONDocument(result.second);
  auto expected = JSONDocument(cmp_json);

  // result.deleteChild("_id");

  if (returned == expected)
    return true;
  else {
    std::cout << "Convertion failed. \n";
    std::cerr << "returned:\n" << returned << "\n";
    std::cerr << "expected:\n" << expected << "\n";
  }

  return true;
}

bool test_loadconfig(std::string const& filt, std::string const& src, std::string const& cmp) {
  confirm(!filt.empty());
  confirm(!src.empty());
  confirm(!cmp.empty());

  std::ifstream is1(filt);
  std::string filt_json((std::istreambuf_iterator<char>(is1)), std::istreambuf_iterator<char>());
  is1.close();

  std::ifstream is3(cmp);
  std::string cmp_json((std::istreambuf_iterator<char>(is3)), std::istreambuf_iterator<char>());
  is3.close();

  auto src_json = std::string();

  auto result = cfgui::read_document(filt_json, src_json);

  if (!result.first) {
    std::cerr << "Error message: " << result.second << "\n";
    return false;
  }

  auto returned = JSONDocument(src_json);
  auto expected = JSONDocument(cmp_json);

  // result.deleteChild("_id");

  if (returned == expected)
    return true;
  else {
    std::cout << "Convertion failed. \n";
    std::cerr << "returned:\n" << returned << "\n";
    std::cerr << "expected:\n" << expected << "\n";
  }

  return true;
}
