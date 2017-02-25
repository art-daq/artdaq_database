#include "test/common.h"

#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/ConfigurationDB/configuration_common.h"
#include "artdaq-database/ConfigurationDB/dispatch_common.h"

#include "artdaq-database/JsonDocument/JSONDocument.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h"
#include "artdaq-database/StorageProviders/MongoDB/provider_mongodb.h"

#include "artdaq-database/DataFormats/Json/json_reader.h"
#include "artdaq-database/DataFormats/shared_literals.h"

#include <libgen.h>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include <iostream>

namespace db = artdaq::database;
namespace cf = db::configuration;
namespace cfo = cf::options;

//namespace literal = artdaq::database::configuration::literal;
namespace bpo = boost::program_options;

using Options = cf::ManageDocumentOperation;

using artdaq::database::docrecord::JSONDocument;
using artdaq::database::basictypes::JsonData;

namespace option {
constexpr auto path = "path";
constexpr auto appname = "ImportDune35tData_t";
}

int write_document_file(Options const&, std::string const&);
std::vector<std::string> list_files(std::string const&);

int main(int argc, char* argv[]) try {
  artdaq::database::configuration::debug::ExportImport();
  artdaq::database::configuration::debug::ManageAliases();
  artdaq::database::configuration::debug::ManageConfigs();
  artdaq::database::configuration::debug::ManageDocuments();
  artdaq::database::configuration::debug::Metadata();
  
  artdaq::database::configuration::debug::detail::ExportImport();
  artdaq::database::configuration::debug::detail::ManageAliases();
  artdaq::database::configuration::debug::detail::ManageConfigs();
  artdaq::database::configuration::debug::detail::ManageDocuments();
  artdaq::database::configuration::debug::detail::Metadata();

  artdaq::database::configuration::debug::options::OperationBase();
  artdaq::database::configuration::debug::options::BulkOperations();
  artdaq::database::configuration::debug::options::ManageDocuments();
  artdaq::database::configuration::debug::options::ManageConfigs();
  artdaq::database::configuration::debug::options::ManageAliases();
  
  artdaq::database::configuration::debug::MongoDB();
  artdaq::database::configuration::debug::FileSystemDB();
  artdaq::database::configuration::debug::UconDB();
  
  artdaq::database::filesystem::debug::enable();
  artdaq::database::mongo::debug::enable();

  artdaq::database::docrecord::debug::JSONDocumentBuilder();
  artdaq::database::docrecord::debug::JSONDocument();

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
  options.format(data_format_t::fhicl);
  options.operation(apiliteral::operation::writedocument);
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
    options.collection(to_collection_name(file_name));    
    if(file_name.rfind(".fcl")!=std::string::npos)
     write_document_file(options,file_name);
  }
} catch (...) {
  std::cout << "Process exited with error: " << ::debug::current_exception_diagnostic_information();
  return process_exit_code::UNCAUGHT_EXCEPTION;
}

int write_document_file(Options const& options, std::string const& file_src_name) try {
  confirm(!file_src_name.empty());
  
  std::cout << "write_document_file file name=<" << file_src_name << ">\n";
  std::cout << "write_document_file operation=<" << options.operation() << ">\n";

        auto test_document=std::string{};
      db::read_buffer_from_file(test_document,file_src_name);


  auto options_string = options.to_string();
  
  using namespace artdaq::database::configuration::json;

  cf::registerOperation<cf::opsig_strstr_t, cf::opsig_strstr_t::FP, std::string const&, std::string&>(
      apiliteral::operation::writedocument, write_document, options_string, test_document);


  auto result = cf::getOperations().at(options.operation())->invoke();

  if (result.first) return process_exit_code::SUCCESS;

  std::cout << "Returned buffer:\n" << result.second << "\n";

  auto file_out_name = std::string(db::filesystem::mkdir(tmpdir)).append("/")
                           .append(option::appname)
                           .append("-")
                           .append(options.operation())
                           .append("-")
                           .append(basename((char*)file_src_name.c_str()))
                           .append(".txt");

  db::write_buffer_to_file(result.second,file_out_name);
			   
  std::cout << "Wrote file:" << file_out_name << "\n";

  return process_exit_code::FAILURE;
} catch (...) {
  std::cout << "Process exited with error: " << ::debug::current_exception_diagnostic_information();
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
