#include "test/common.h"

#include "artdaq-database/BasicTypes/data_json.h"
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

namespace bpo = boost::program_options;

using Options = cf::ManageDocumentOperation;

int write_document(Options const& /*options*/, std::string const& /*file_name*/);
int configuration_composition(Options const& /*options*/, std::string& /*confcomp*/);

int main(int argc, char* argv[]) try {
  auto start = std::chrono::high_resolution_clock::now();

  std::ostringstream descstr;
  descstr << argv[0] << " <-p <path>> <other-options>";

  bpo::options_description desc = descstr.str();

  desc.add_options()("path,p", bpo::value<std::string>(), "Path to fhicl files.")("run,r", bpo::value<std::size_t>(), "Run number.")(
      "threads,t", bpo::value<std::size_t>(), "Thread count.")("debug,d", bpo::value<bool>(), "Read configuration from database.")(
      "configuration,c", bpo::value<std::string>(), "Configuration Name.")("help,h", "produce help message");

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

  auto debug = bool{false};

  if (vm.count("debug") != 0u) {
    debug = vm[apiliteral::option::debug].as<bool>();
  }

  using cfo::data_format_t;
  auto options1 = Options{argv[0]};

  options1.format(data_format_t::fhicl);
  options1.operation(apiliteral::operation::writedocument);

  auto dir_path = vm[apiliteral::option::path].as<std::string>();
  auto configuration = vm[apiliteral::option::configuration].as<std::string>();
  auto run = std::to_string(vm[apiliteral::option::run].as<std::size_t>());

  options1.configuration(run + '/' + configuration);
  options1.run(run);
  options1.version(run + '/' + configuration);

  auto file_names = db::list_files(dir_path);
  std::mutex file_names_mutex;

  std::atomic<int> loaded_file_count{0};
  std::vector<std::thread> workers;

  auto const stropts = options1.writeJsonData();

  for (std::size_t i = 0; i < nproc; i++) {
    workers.emplace_back([&file_names, &file_names_mutex, &loaded_file_count, stropts]() {
      auto file_name = std::string{};
      auto entity = std::string{jsonliteral::notprovided};
      auto options = Options{"Worker"};
      int filecount = 0;
      options.readJsonData(stropts);
      while (true) {
        {
          std::lock_guard<std::mutex> lock(file_names_mutex);
          if (file_names.empty()) {
            loaded_file_count.fetch_add(filecount);
            return;
          }
          file_name = file_names.back();
          file_names.pop_back();
        }

        options.collection("RunHistory");

        if (file_name.rfind(".fcl") != std::string::npos) {
          if (file_name.rfind("schema.fcl") == std::string::npos) {
            boost::filesystem::path p(file_name);
            entity = p.stem().string();
            options.entity(entity);
          } else {
            options.collection("SystemLayout");
            options.entity(jsonliteral::schema);
          }
          write_document(options, file_name);
          filecount++;
        }
      }
    });
  }

  for (auto& worker : workers) {
    worker.join();
  }

  auto elapsed_time = std::chrono::high_resolution_clock::now() - start;

  std::stringstream oss;

  oss << "Loaded " << loaded_file_count << " files with " << workers.size();
  oss << " threads in " << std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_time).count() << " msecs.\n";
  oss << "Avarage file load time is ";
  oss << std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_time).count() *
             std::min((static_cast<size_t>(std::thread::hardware_concurrency())), workers.size()) / loaded_file_count;
  oss << " msecs.\n";

  std::cout << oss.str();

  if (debug) {
    auto options1 = Options{argv[0]};
    options1.operation(apiliteral::operation::confcomposition);
    options1.configuration(run + "/" + configuration);
    options1.format(data_format_t::gui);
    auto confcomp = std::string{};
    auto result = configuration_composition(options1, confcomp);
    if (result == 0) {
      return process_exit_code::FAILURE;
    }
  }

  return process_exit_code::SUCCESS;
} catch (...) {
  std::cerr << "Process exited with exception: " << boost::current_exception_diagnostic_information();
  return process_exit_code::UNCAUGHT_EXCEPTION;
}

int write_document(Options const& options, std::string const& file_name) try {
  confirm(!file_name.empty());
  auto test_document = std::string{};
  db::read_buffer_from_file(test_document, file_name);

  auto result = cf::opts::write_document(options, test_document);

  if (result.first) {
    return process_exit_code::SUCCESS;
  }

  std::cerr << "Failed loading file:" << file_name << "\n";
  std::cerr << "Error message:" << result.second << "\n";

  return process_exit_code::FAILURE;
} catch (...) {
  std::cerr << "Failed loading file; exception: " << boost::current_exception_diagnostic_information();
  return process_exit_code::UNCAUGHT_EXCEPTION;
}

int configuration_composition(Options const& options, std::string& confcomp) try {
  auto result = cf::opts::configuration_composition(options);

  if (result.first) {
    std::cout << "Configuration composition json=<" << result.second << ">\n";
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
