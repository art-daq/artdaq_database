#include "test/common.h"

#include "artdaq-database/ConfigurationDB/common.h"
#include "artdaq-database/ConfigurationDB/conftoolifc.h"
/*
#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/ConfigurationDB/configuration_common.h"
#include "artdaq-database/ConfigurationDB/dispatch_common.h"

#include "artdaq-database/JsonDocument/JSONDocument.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h"
#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb_index.h"

#include "artdaq-database/StorageProviders/MongoDB/provider_mongodb.h"

#include "artdaq-database/DataFormats/Json/json_reader.h"
*/

namespace db = artdaq::database;
namespace bpo = boost::program_options;
namespace impl = db::configuration::json;


using Options = db::configuration::ManageDocumentOperation;


int main(int argc, char* argv[]) try {
#if 1

  impl::enable_trace();

  debug::registerUngracefullExitHandlers();
  artdaq::database::useFakeTime(true);
#endif

  //artdaq::database::filesystem::index::shouldAutoRebuildSearchIndex(true);

  auto options = std::make_unique<Options>(argv[0]);

  auto result = db::conftool_impl(options, argc, argv);

  if (!result.first) {
    std::cout << "Failed; error message: " << result.second << "\n";
    std::cout << ::debug::current_exception_diagnostic_information();
    return process_exit_code::FAILURE;
  }

  auto returned = std::string{result.second};

  if (options->operation().compare(apiliteral::operation::readdocument) == 0 ||
      options->operation().compare(apiliteral::operation::readconfiguration) == 0) {
    db::write_buffer_to_file(returned, options->resultFileName());
    std::cout << "Wrote file:" << options->resultFileName() << "\n";
  
    return process_exit_code::SUCCESS;
  } else if (options->operation().compare(apiliteral::operation::writedocument) == 0 ||
             options->operation().compare(apiliteral::operation::writeconfiguration) == 0) {
    /* db::write_buffer_to_file(returned,options->resultFileName());
     std::cout << "Wrote responce to file:" << file_res_name << "\n";
     */
    return process_exit_code::SUCCESS;
  }

  std::cout << "Results:" << returned << "\n";
  return process_exit_code::SUCCESS;

} catch (...) {
  std::cout << "Process exited with error: " << ::debug::current_exception_diagnostic_information();
  return process_exit_code::UNCAUGHT_EXCEPTION;
}
