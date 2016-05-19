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

#include "artdaq-database/DataFormats/Fhicldb.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h"
#include "artdaq-database/StorageProviders/MongoDB/provider_mongodb.h"
#include "artdaq-database/DataFormats/convertjson2guijson.h"

#include <boost/exception/diagnostic_information.hpp>

namespace bpo = boost::program_options;

namespace db = artdaq::database;
namespace cf = db::configuration;
namespace cfo = cf::options;
namespace gui = cf::guiexports;
namespace cfol = cfo::literal;

// std::string operator"" _s(const char* text, std::size_t) { return std::string(text) ; }

int main(int argc, char* argv[]) try {
  debug::registerUngracefullExitHandlers();
  //    artdaq::database::fhicljson::useFakeTime(true);

  cf::trace_enable_CreateConfigsOperation();
  cf::trace_enable_CreateConfigsOperationDetail();

  cf::trace_enable_LoadStoreOperation();
  cf::trace_enable_FindConfigsOperationDetail();

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
  db::fhicljson::trace_enable_convertjson2guijson();
  
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

  assert(!config_file.empty());
  assert(!json_file.empty());

  std::cout << "Running :<" << operation << ">\n";

  std::ifstream is(config_file);
  std::string search_filter((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
  is.close();

  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>("load", load_configuration,
                                                                                  search_filter);
  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(
      "findconfigs", find_global_configurations, search_filter);
  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(
      "buildfilter", build_global_configuration_search_filter, search_filter);
  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(
      "addconfig", add_configuration_to_global_configuration, search_filter);
  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(
      "newconfig", create_new_global_configuration, search_filter);
  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(
      "findversions", find_configuration_versions, search_filter);
  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>(
      "findentities", find_configuration_entities, search_filter);
  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>("addalias", add_version_alias,
                                                                                  search_filter);
  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>("removealias", remove_version_alias,
                                                                                  search_filter);
  cf::registerOperation<cf::opsig_str_t, cf::opsig_str_t::FP, std::string const&>("findaliases", find_version_aliases,
                                                                                  search_filter);

  auto json_document = std::string{"{}"};

  try {
    std::ifstream is1(json_file);
    json_document = std::string((std::istreambuf_iterator<char>(is1)), std::istreambuf_iterator<char>());
    is1.close();
    cf::registerOperation<cf::opsig_strstr_t, cf::opsig_strstr_t::FP, std::string const&, std::string const&>(
        "store", store_configuration, search_filter, json_document);
  } catch (...) {
  }

  auto result = cf::getOperations().at(operation)->invoke();

  if (!result.first) {
    std::cerr << "Error message: " << result.second << "\n";
    return process_exit_code::FAILURE;
  }

  std::ofstream os(json_file + ".out");
  std::copy(result.second.begin(), result.second.end(), std::ostream_iterator<char>(os));
  os.close();

  return process_exit_code::SUCCESS;

} catch (...) {
  std::cerr << "Process exited with error: " << boost::current_exception_diagnostic_information();
  return process_exit_code::UNCAUGHT_EXCEPTION;
}
