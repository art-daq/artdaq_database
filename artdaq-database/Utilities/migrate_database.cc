#include "test/common.h"

#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/ConfigurationDB/dboperation_metadata.h"
#include "artdaq-database/JsonDocument/JSONDocument.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/JsonDocument/JSONDocumentMigrator.h"

#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h"

namespace bpo = boost::program_options;
using namespace artdaq::database;

using namespace artdaq::database::configuration;
namespace cf = artdaq::database::configuration;

namespace DBI = artdaq::database::filesystem;

using artdaq::database::basictypes::JsonData;
using artdaq::database::docrecord::JSONDocumentBuilder;
using artdaq::database::docrecord::JSONDocumentMigrator;
using artdaq::database::docrecord::JSONDocument;

int main(int argc, char* argv[]) try {
  std::ostringstream descstr;
  descstr << argv[0] << " <-uri <database uri>>";

  bpo::options_description desc = descstr.str();

  desc.add_options()("uri,u", bpo::value<std::string>(), "Database URI")("help,h", "produce help message");

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

  if (!vm.count("uri")) {
    std::cerr << "Exception from command line processing in " << argv[0] << ": no databse URI given.\n"
              << "For usage and an options list, please do '" << argv[0] << " --help"
              << "'.\n";
    return process_exit_code::INVALID_ARGUMENT | 1;
  }

  auto database_uri = vm["uri"].as<std::string>();

  if (database_uri.find(DBI::literal::FILEURI) != 0) {
    std::cerr << "Only the filesystem database provider is supported.";
    return process_exit_code::INVALID_ARGUMENT | 1;
  }

  if (database_uri.back() == '/') database_uri.pop_back();  // remove trailing slash

  auto database_path = database_uri.substr(strlen(DBI::literal::FILEURI));
  database_path = db::expand_environment_variables(database_path);

  auto config = DBI::DBConfig{database_uri + "_migrated"};
  auto database = DBI::DB::create(config);
  auto provider = DBI::DBProvider<JsonData>::create(database);

  std::ostringstream oss;

  std::cout << "Database:" << database_path << "\n";

  auto collection_names = DBI::find_subdirs(database_path);
  for (auto const& collection_name : collection_names) {
    if (collection_name == system_metadata) continue;

    std::cout << "\n\n Collection:" << collection_name << "\n";

    boost::filesystem::directory_iterator end_iter;

    auto collection_path = database_path + "/" + collection_name;

    for (boost::filesystem::directory_iterator dir_iter(collection_path); dir_iter != end_iter; ++dir_iter) {
      if (boost::filesystem::is_directory(dir_iter->status()) || dir_iter->path().filename() == "index.json") continue;
      auto oid = dir_iter->path().filename().replace_extension().string();

      std::cout << "   Document: " << oid;

      try {
        auto source = std::string{};

        if (!db::read_buffer_from_file(source, dir_iter->path().string())) {
          std::cerr << " Unable to read a file " << dir_iter->path();
          std::cout << " -> failed\n";
          continue;
        }

        JSONDocumentBuilder builder{JSONDocumentMigrator{{source}}};
        builder.setCollection({"{db::quoted_(apiliteral::option::collection):\""s + collection_name + "\"}"});

        oss.str("");
        oss.clear();
        oss << "{" << quoted_(jsonliteral::document) << ":" << builder << ",";
        oss << quoted_(jsonliteral::filter) << ":" << to_id(oid) << ",";
        oss << quoted_(jsonliteral::collection) << ":" << quoted_(collection_name) << "}";

        provider->writeDocument(oss.str());
        std::cout << " -> succeeded\n";

      } catch (...) {
        std::cerr << "Failed to import a document: " << ::debug::current_exception_diagnostic_information() << "\n";
      }
    }
  }

  return process_exit_code::SUCCESS;
} catch (...) {
  std::cerr << "Process exited with error: " << ::debug::current_exception_diagnostic_information();
  return process_exit_code::UNCAUGHT_EXCEPTION;
}
