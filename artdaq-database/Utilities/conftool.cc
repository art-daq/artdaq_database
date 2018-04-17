#include "test/common.h"

#include "artdaq-database/ConfigurationDB/common.h"
#include "artdaq-database/ConfigurationDB/conftoolifc.h"

namespace db = artdaq::database;
namespace impl = db::configuration::json;

using Options = db::configuration::ManageDocumentOperation;

int main(int argc, char* argv[]) try {
#if 1

  impl::enable_trace();

  debug::registerUngracefullExitHandlers();
  artdaq::database::useFakeTime(true);
#endif

  // artdaq::database::filesystem::index::shouldAutoRebuildSearchIndex(true);

  auto options = std::make_unique<Options>(argv[0]);

  auto result = db::conftool_impl(options, argc, argv);

  if (!result.first) {
    std::cout << "Failed; error message: " << result.second << "\n";
    std::cout << ::debug::current_exception_diagnostic_information();
    return process_exit_code::FAILURE;
  }

  auto returned = std::string{result.second};

  if (options->operation() == apiliteral::operation::readdocument ||
      options->operation() == apiliteral::operation::readconfiguration) {
    db::write_buffer_to_file(returned, options->resultFileName());
    std::cout << "Wrote file:" << options->resultFileName() << "\n";

    return process_exit_code::SUCCESS;
  } else if (options->operation() == apiliteral::operation::writedocument ||
             options->operation() == apiliteral::operation::writeconfiguration) {
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
