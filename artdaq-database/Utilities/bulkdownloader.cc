#include "test/common.h"

#include "artdaq-database/BasicTypes/data_json.h"
#include "artdaq-database/DataFormats/Json/json_common.h"
#include "artdaq-database/DataFormats/Json/json_types_impl.h"

#include "artdaq-database/ConfigurationDB/configuration_common.h"
#include "artdaq-database/ConfigurationDB/dboperation_managedocument.h"

#include <libgen.h>
#include <atomic>
#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

#include <boost/exception/diagnostic_information.hpp>
#include <boost/filesystem.hpp>

namespace db = artdaq::database;
namespace cf = db::configuration;
namespace cfo = cf::options;
namespace jsn = db::json;

namespace bpo = boost::program_options;

using Options = cf::ManageDocumentOperation;

int read_document(Options const& /*options*/, std::string const& /*file_name*/);
int configuration_composition(Options const& /*options*/, std::string& /*confcomp*/);

int main(int argc, char* argv[]) try {
  auto start = std::chrono::high_resolution_clock::now();

  std::ostringstream descstr;
  descstr << argv[0] << " <-p <path>> <other-options>";

  bpo::options_description desc = descstr.str();

  desc.add_options()
    ("path,p", bpo::value<std::string>(), "Path to fhicl files.")
    ("run,r", bpo::value<std::size_t>(), "Run number.")
    ("threads,t", bpo::value<std::size_t>(), "Thread count.")
    ("configuration,c", bpo::value<std::string>(), "Configuration Name.")
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

  if (vm.count("path") == 0u) {
    std::cerr << "Exception from command line processing in " << argv[0] << ": no path to fhicl files given.\n"
              << "For usage and an options list, please do '" << argv[0] << " --help"
              << "'.\n";
    return process_exit_code::INVALID_ARGUMENT | 1;
  }

  if (vm.count("configuration") == 0u) {
    std::cerr << "Exception from command line processing in " << argv[0] << ": no configuration name given.\n"
              << "For usage and an options list, please do '" << argv[0] << " --help"
              << "'.\n";
    return process_exit_code::INVALID_ARGUMENT | 2;
  }
  if (vm.count("run") == 0u) {
    std::cerr << "Exception from command line processing in " << argv[0] << ": no run number given.\n"
              << "For usage and an options list, please do '" << argv[0] << " --help"
              << "'.\n";
    return process_exit_code::INVALID_ARGUMENT | 3;
  }

  std::size_t nproc = std::thread::hardware_concurrency();

  if (vm.count("threads") != 0u) {
    nproc = vm[apiliteral::option::threads].as<std::size_t>();
  }

  auto dir_path = vm[apiliteral::option::path].as<std::string>();
  auto configuration = vm[apiliteral::option::configuration].as<std::string>();
  auto run = std::to_string(vm[apiliteral::option::run].as<std::size_t>());
  auto confprefix=configuration.substr(0,configuration.find_last_not_of("0123456789")+1);

  auto options1 = Options{argv[0]};
  options1.operation(apiliteral::operation::confcomposition);
  options1.configuration(run + "/" + configuration);
  options1.format(cfo::data_format_t::gui);
  auto confcomp = std::string{};
  auto result = configuration_composition(options1, confcomp);
  if (result != process_exit_code::SUCCESS) {
      return result;
  }

  std::vector<std::thread> workers;

  auto ast= jsn::object_t{};

  if(!jsn::JsonReader().read(confcomp,ast)){
     return process_exit_code::FAILURE;
  }
  
  auto const& results = unwrap(ast).value_as<const jsn::array_t>(jsonliteral::search);

  auto writer= jsn::JsonWriter{};
  auto buff= std::string{};
  
  auto queries = std::vector<std::pair<std::string,std::string>>{};

  for(auto& result : results){
       buff.clear();
       auto const& query = unwrap(result).value_as<const jsn::object_t>(jsonliteral::query);
       auto const& filter = unwrap(query).value_as<const jsn::object_t>(apiliteral::option::searchfilter );
       auto const& collection = unwrap(query).value_as<const std::string>(apiliteral::option::collection);
       auto const& entity = unwrap(filter).value_as<const std::string>(apiliteral::filter::entities);
       if(!writer.write(query,buff)){
           std::cerr << "Failed serializing query\n";
            return process_exit_code::FAILURE;
       }
       auto file_name= std::string{dir_path};
       if(file_name.back()!='/') file_name+="/";
       
       if(collection!="SystemLayout"){
        file_name+=confprefix;
        file_name+="/";
       }
       
       file_name+=entity;
       file_name+=".fcl";
       queries.emplace_back(file_name,buff);
  }


  std::mutex queries_mutex;  
  auto file_names = std::vector<std::string>{};
  std::mutex file_names_mutex;
  
  for (std::size_t i = 0; i < nproc; i++) {
    workers.emplace_back([&queries, &queries_mutex, &file_names, &file_names_mutex]() {
      auto my_file_names = std::vector<std::string>{};
      auto file_name=std::string{};
      auto query=std::string{};

      auto options = Options{"Worker"};
      while (true) {
        {
          std::lock_guard<std::mutex> lock(queries_mutex);
          if (queries.empty()) {
            std::lock_guard<std::mutex> lock(file_names_mutex);
            std::copy (my_file_names.begin(), my_file_names.end(), std::back_inserter(file_names));
            return;
          }
          file_name = std::move(queries.back().first);
          query = std::move(queries.back().second);
          queries.pop_back();
        }
        options.readJsonData(query);
        options.format(cfo::data_format_t::fhicl);
        read_document(options, file_name);
        my_file_names.emplace_back(file_name);
        }
    });
  }

  for (auto& worker : workers) {
    worker.join();
  }

  auto elapsed_time = std::chrono::high_resolution_clock::now() - start;

  std::stringstream oss;

  oss << "Downloaded " << file_names.size() << " files with " << workers.size();
  oss << " threads in " << std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_time).count() << " msecs.\n";
  oss << "Avarage file load time is ";
  oss << std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_time).count() *
             std::min((static_cast<size_t>(std::thread::hardware_concurrency())), workers.size()) / file_names.size();
  oss << " msecs.\n";

  std::cout << oss.str();

  return process_exit_code::SUCCESS;
} catch (...) {
  std::cerr << "Process exited with exception: " << boost::current_exception_diagnostic_information();
  return process_exit_code::UNCAUGHT_EXCEPTION;
}

int read_document(Options const& options, std::string const& file_name) try {
  confirm(!file_name.empty());
  auto test_document = std::string{};
  auto result = cf::opts::read_document(options, test_document);
  if (result.first && db::write_buffer_to_file(test_document, file_name)) {
    return process_exit_code::SUCCESS;  
  }

  std::cerr << "Failed writing file:" << file_name << "\n";
  std::cerr << "Error message:" << result.second << "\n";

  return process_exit_code::FAILURE;
} catch (...) {
  std::cerr << "Failed writing file; exception: " << boost::current_exception_diagnostic_information();
  return process_exit_code::UNCAUGHT_EXCEPTION;
}

int configuration_composition(Options const& options, std::string& confcomp) try {
  auto result = cf::opts::configuration_composition(options);

  if (result.first) {
    std::swap(result.second, confcomp);
    return process_exit_code::SUCCESS;
  }

  std::cerr << "Failed loading configuration composition for " << options.configuration() << "\n";
  std::cerr << "Error message:" << result.second << "\n";

  return process_exit_code::FAILURE;
} catch (...) {
  std::cerr << "Failed reading configuration composition; exception: " << boost::current_exception_diagnostic_information();
  return process_exit_code::UNCAUGHT_EXCEPTION;
}
