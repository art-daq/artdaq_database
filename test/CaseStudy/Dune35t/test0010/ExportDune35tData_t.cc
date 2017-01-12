#include "test/common.h"

#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/ConfigurationDB/configuration_common.h"
#include "artdaq-database/ConfigurationDB/dispatch_common.h"

#include "artdaq-database/JsonDocument/JSONDocument.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h"
#include "artdaq-database/StorageProviders/MongoDB/provider_mongodb.h"

#include "artdaq-database/DataFormats/Json/json_reader.h"
#include "artdaq-database/DataFormats/common/helper_functions.h"
#include "artdaq-database/DataFormats/shared_literals.h"

#include <libgen.h>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include <iostream>

namespace db = artdaq::database;
namespace cf = db::configuration;
namespace cfo = cf::options;

namespace literal = artdaq::database::configuration::literal;
namespace bpo = boost::program_options;

using Options = cf::LoadStoreOperation;

using artdaq::database::docrecord::JSONDocument;
using artdaq::database::basictypes::JsonData;

namespace option {
constexpr auto path = "path";
constexpr auto appname = "ExportDune35tData_t";
}

int store_configuration_file(Options const&, std::string const&);
std::vector<std::string> list_files(std::string const&);

int main(int argc, char* argv[]) try {
  artdaq::database::filesystem::debug::enable();
  artdaq::database::mongo::debug::enable();
  // artdaq::database::docrecord::debug::enableJSONDocument();
  // artdaq::database::docrecord::debug::enableJSONDocumentBuilder();

  artdaq::database::configuration::debug::enableFindConfigsOperation();
  artdaq::database::configuration::debug::enableCreateConfigsOperation();

  artdaq::database::configuration::debug::options::enableOperationBase();
  artdaq::database::configuration::debug::options::enableOperationManageConfigs();
  artdaq::database::configuration::debug::detail::enableCreateConfigsOperation();
  artdaq::database::configuration::debug::detail::enableFindConfigsOperation();

  artdaq::database::configuration::debug::enableDBOperationMongo();
  artdaq::database::configuration::debug::enableDBOperationFileSystem();

  debug::registerUngracefullExitHandlers();
  artdaq::database::useFakeTime(true);

  auto options = Options{argv[0]};
  auto desc = options.makeProgramOptions();

  bpo::variables_map vm;

  try {
    bpo::store(bpo::command_line_parser(argc, argv).options(desc).run(), vm);
    bpo::notify(vm);
  } catch (bpo::error const& e) {
    std::cerr << "Exception from command line processing in " << argv[0] << ": " << e.what() << "\n";
    return process_exit_code::INVALID_ARGUMENT;
  }

  if (vm.count("help")) {
    std::cerr << desc << std::endl;
    return process_exit_code::HELP;
  }

  auto exit_code = options.readProgramOptions(vm);
  if (exit_code != process_exit_code::SUCCESS) {
    std::cerr << desc << std::endl;
    return exit_code;
  }

  using cfo::data_format_t;
  options.dataFormat(data_format_t::fhicl);
  options.operation(apiliteral::operation::store);
  auto path_to_config_dir = vm[apiliteral::option::source].as<std::string>();

  auto to_collection_name = [](std::string const& file_path_str) ->std::string {
    auto  file_path_copy= std::string{file_path_str.c_str()};
    char* file_path = (char*)(file_path_copy.c_str());
    auto names = std::list<std::string>{};
    names.push_front(basename(file_path));
    char* parent = dirname(file_path);
    names.push_front(basename(parent));

//  parent = dirname(parent);
//  names.push_front(basename(parent));

    std::ostringstream oss;
    for (auto const& name : names) oss << name << ".";
    auto collName = oss.str();
    collName.pop_back();

    return collName;
  };

  for (auto const& file_name : list_files(path_to_config_dir)) {
    options.collectionName(to_collection_name(file_name));    
    if(file_name.rfind(".fcl")!=std::string::npos)
     store_configuration_file(options,file_name);
  }
} catch (...) {
  std::cout << "Process exited with error: " << boost::current_exception_diagnostic_information();
  return process_exit_code::UNCAUGHT_EXCEPTION;
}

int store_configuration_file(Options const& options, std::string const& file_src_name) try {
  confirm(!file_src_name.empty());
  
  std::cout << "store_configuration_file file name=<" << file_src_name << ">\n";
  std::cout << "store_configuration_file operation=<" << options.operation() << ">\n";

  std::ifstream is(file_src_name);
  auto test_document = std::string((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
  is.close();

  auto options_string = options.to_string();
  
  using namespace artdaq::database::configuration::json;

  cf::registerOperation<cf::opsig_strstr_t, cf::opsig_strstr_t::FP, std::string const&, std::string&>(
      apiliteral::operation::store, store_configuration, options_string, test_document);


  auto result = cf::getOperations().at(options.operation())->invoke();

  if (result.first) return process_exit_code::SUCCESS;

  std::cout << "Returned buffer:\n" << result.second << "\n";

  auto file_out_name = std::string(db::filesystem::mkdir(tmpdir))
                           .append(option::appname)
                           .append("-")
                           .append(options.operation())
                           .append("-")
                           .append(basename((char*)file_src_name.c_str()))
                           .append(".txt");

  std::ofstream os(file_out_name.c_str());
  std::copy(result.second.begin(), result.second.end(), std::ostream_iterator<char>(os));
  os.close();

  std::cout << "Wrote file:" << file_out_name << "\n";

  return process_exit_code::FAILURE;
} catch (...) {
  std::cout << "Process exited with error: " << boost::current_exception_diagnostic_information();
  return process_exit_code::UNCAUGHT_EXCEPTION;
}

std::vector<std::string> list_files(std::string const& path) {
  auto files = std::vector<std::string>{};
  files.reserve(1024);

  for (auto& entry : boost::make_iterator_range(boost::filesystem::directory_iterator(path), {})) {
    if (boost::filesystem::is_regular(entry))
      files.push_back(entry.path().string());
    else if (boost::filesystem::is_directory(entry.path())) {
      auto suddir_list = list_files(entry.path().string());
      std::copy(suddir_list.begin(), suddir_list.end(), std::back_inserter(files));
    }
  }

  return files;
}
