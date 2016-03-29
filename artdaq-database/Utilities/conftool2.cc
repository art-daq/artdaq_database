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

#include "artdaq-database/FhiclJson/fhicljsondb.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h"
#include "artdaq-database/StorageProviders/MongoDB/provider_mongodb.h"

#include <boost/exception/diagnostic_information.hpp>

namespace bpo = boost::program_options;

namespace db = artdaq::database;
namespace cf = db::configuration;
namespace cfo = cf::options;
namespace gui = cf::guiexports;
namespace cfol = cfo::literal;

typedef bool (*test_case)(std::string const& /*filter*/, std::string const& /*json*/);

bool test_storeconfig(std::string const&, std::string const&);
bool test_loadconfig(std::string const&, std::string const&);
bool test_findconfigs(std::string const&, std::string const&);
bool test_buildfilter(std::string const&, std::string const&);
bool test_findversions(std::string const&, std::string const&);
bool test_addconfig(std::string const&, std::string const&);
bool test_newconfig(std::string const&, std::string const&);
bool test_findentities(std::string const&, std::string const&);

int main(int argc, char* argv[]) try {
  debug::registerUngracefullExitHandlers();
  //    artdaq::database::fhicljson::useFakeTime(true);
  
  
  cf::trace_enable_CreateConfigsOperation();
  cf::trace_enable_CreateConfigsOperationDetail();

  cf::trace_enable_LoadStoreOperation();
  cf::trace_enable_FindConfigsOperationDetail();

  cf::trace_enable_FindConfigsOperation();
  cf::trace_enable_FindConfigsOperationDetail();

  /*
  cf::trace_enable_DBOperationMongo();
  cf::trace_enable_DBOperationFileSystem();
  

  db::filesystem::trace_enable();
  db::mongo::trace_enable();

  db::fhicljson::trace_enable_fcl2jsondb();
  db::fhicl::trace_enable_FhiclReader();
  db::fhicl::trace_enable_FhiclWriter();
  db::fhicljsondb::trace_enable_fhicljsondb();

  db::jsonutils::trace_enable_JSONDocumentBuilder();
  */
  
  // Get the input parameters via the boost::program_options library,
  // designed to make it relatively simple to define arguments and
  // issue errors if argument list is supplied incorrectly

  namespace db = artdaq::database;
  namespace cfol = artdaq::database::configuration::options::literal;
  std::ostringstream descstr;
  descstr << argv[0] << " <-o <operation>>  <-c <config-file>> <-j <json-file>> ";

  bpo::options_description desc = descstr.str();

  desc.add_options()("operation,o", bpo::value<std::string>(), "Operation [load/store/findconfigs/buildfilter].")(
      "configuration,c", bpo::value<std::string>(), "Configuration file name.")(
      "json,j", bpo::value<std::string>(), "JSON source file name.")("help,h", "produce help message");

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

  if (!vm.count("operation")) {
    std::cerr << "Exception from command line processing in " << argv[0] << ": no database operation given.\n"
              << "For usage and an options list, please do '" << argv[0] << " --help"
              << "'.\n";
    return process_exit_code::INVALID_ARGUMENT | 1;
  }

  auto operation = vm["operation"].as<std::string>();

  if (!vm.count("configuration")) {
    std::cerr << "Exception from command line processing in " << argv[0] << ": no file name given.\n"
              << "For usage and an options list, please do '" << argv[0] << " --help"
              << "'.\n";
    return process_exit_code::INVALID_ARGUMENT | 2;
  }
  auto config_file = vm["configuration"].as<std::string>();

  if (!vm.count("json")) {
    std::cerr << "Exception from command line processing in " << argv[0] << ": no file name given.\n"
              << "For usage and an options list, please do '" << argv[0] << " --help"
              << "'.\n";
    return process_exit_code::INVALID_ARGUMENT | 2;
  }
  auto json_file = vm["json"].as<std::string>();

  std::cout << "Running :<" << operation << ">\n";

  auto runTest = [](std::string const& name) {
    auto tests = std::map<std::string, test_case>{{"store", test_storeconfig},        {"load", test_loadconfig},
                                                  {"findconfigs", test_findconfigs},  {"buildfilter", test_buildfilter},
                                                  {"addconfig", test_addconfig},      {"newconfig", test_newconfig},
                                                  {"findversions", test_findversions}, {"findentities", test_findentities}};

    return tests.at(name);
  };

  auto testResult = runTest(operation)(config_file, json_file);

  if (testResult) return process_exit_code::SUCCESS;

  return process_exit_code::FAILURE;
} catch (...) {
  std::cerr << "Process exited with error: " << boost::current_exception_diagnostic_information();
  return process_exit_code::UNCAUGHT_EXCEPTION;
}

bool test_storeconfig(std::string const& config_name, std::string const& json_name) {
  assert(!config_name.empty());
  assert(!json_name.empty());

  std::ifstream is(config_name);

  std::string search_filter((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());

  is.close();

  std::ifstream is1(json_name);

  std::string json_document((std::istreambuf_iterator<char>(is1)), std::istreambuf_iterator<char>());

  is1.close();

  auto result = store_configuration(search_filter, json_document);

  if (!result.first) {
    std::cerr << "Error message: " << result.second << "\n";
    return false;
  }

  std::ofstream os(json_name + ".out");
  std::copy(result.second.begin(), result.second.end(), std::ostream_iterator<char>(os));

  os.close();

  return true;
}

bool test_loadconfig(std::string const& config_name, std::string const& json_name) {
  assert(!config_name.empty());
  assert(!json_name.empty());

  std::ifstream is(config_name);

  std::string search_filter((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());

  is.close();

  auto result = load_configuration(search_filter);

  if (!result.first) {
    return false;
  }

  std::ofstream os(json_name);
  std::copy(result.second.begin(), result.second.end(), std::ostream_iterator<char>(os));

  os.close();

  return true;
}

bool test_findconfigs(std::string const& config_name, std::string const& json_name) {
  assert(!config_name.empty());
  assert(!json_name.empty());

  std::ifstream is(config_name);

  std::string search_filter((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());

  is.close();

  auto result = find_global_configurations(search_filter);

  if (!result.first) {
    return false;
  }

  std::ofstream os(json_name);
  std::copy(result.second.begin(), result.second.end(), std::ostream_iterator<char>(os));

  os.close();

  return true;
}

bool test_buildfilter(std::string const& config_name, std::string const& json_name) {
  assert(!config_name.empty());
  assert(!json_name.empty());

  std::ifstream is(config_name);

  std::string search_filter((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());

  is.close();

  auto result = build_global_configuration_search_filter(search_filter);

  if (!result.first) {
    std::cerr << "Error message: " << result.second << "\n";
    return false;
  }

  std::ofstream os(json_name);
  std::copy(result.second.begin(), result.second.end(), std::ostream_iterator<char>(os));

  os.close();

  return true;
}

bool test_findversions(std::string const& config_name, std::string const& json_name) {
  assert(!config_name.empty());
  assert(!json_name.empty());

  std::ifstream is(config_name);

  std::string search_filter((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());

  is.close();

  auto result = find_configuration_versions(search_filter);

  if (!result.first) {
    std::cerr << "Error message: " << result.second << "\n";
    return false;
  }

  std::ofstream os(json_name);
  std::copy(result.second.begin(), result.second.end(), std::ostream_iterator<char>(os));

  os.close();

  return true;
}

bool test_findentities(std::string const& config_name, std::string const& json_name) {
  assert(!config_name.empty());
  assert(!json_name.empty());

  std::ifstream is(config_name);

  std::string search_filter((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());

  is.close();

  auto result = find_configuration_entities(search_filter);

  if (!result.first) {
    std::cerr << "Error message: " << result.second << "\n";
    return false;
  }

  std::ofstream os(json_name);
  std::copy(result.second.begin(), result.second.end(), std::ostream_iterator<char>(os));

  os.close();

  return true;
}


bool test_addconfig(std::string const& config_name, std::string const& json_name) {
  assert(!config_name.empty());
  assert(!json_name.empty());

  std::ifstream is(config_name);

  std::string search_filter((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());

  is.close();

  auto result = add_configuration_to_global_configuration(search_filter);

  if (!result.first) {
    std::cerr << "Error message: " << result.second << "\n";
    return false;
  }

  std::ofstream os(json_name);
  std::copy(result.second.begin(), result.second.end(), std::ostream_iterator<char>(os));

  os.close();

  return true;
}

bool test_newconfig(std::string const& config_name, std::string const& json_name) {
  assert(!config_name.empty());
  assert(!json_name.empty());

  std::ifstream is(config_name);

  std::string search_filter((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());

  is.close();

  auto result = create_new_global_configuration(search_filter);

  if (!result.first) {
    std::cerr << "Error message: " << result.second << "\n";
    return false;
  }

  std::ofstream os(json_name);
  std::copy(result.second.begin(), result.second.end(), std::ostream_iterator<char>(os));

  os.close();

  return true;
}