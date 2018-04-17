#include "test/common.h"

#include "artdaq-database/ConfigurationDB/common.h"
#include "artdaq-database/ConfigurationDB/conftoolifc.h"

#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/DataFormats/Json/json_reader.h"
#include "artdaq-database/JsonDocument/JSONDocument.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"

namespace db = artdaq::database;
namespace bpo = boost::program_options;
namespace impl = db::configuration::json;

using artdaq::database::docrecord::JSONDocument;
using artdaq::database::docrecord::JSONDocumentBuilder;
namespace ovl = artdaq::database::overlay;

namespace apiliteral = db::configapi::literal;

int main(int argc, char* argv[]) try {
#if 1

  impl::enable_trace();

  debug::registerUngracefullExitHandlers();
  // artdaq::database::useFakeTime(true);
#endif

  std::unique_ptr<db::configuration::OperationBase> options;
  // artdaq::database::filesystem::index::shouldAutoRebuildSearchIndex(true);

  auto make_opt_name = [](auto& long_name, auto& short_name) {
    return std::string{long_name}.append(",").append(short_name);
  };

  bpo::variables_map vm;

  bpo::options_description opts;
  opts.add_options()("help,h", "Produce help message");
  opts.add_options()(make_opt_name(apiliteral::option::operation, "o").c_str(), bpo::value<std::string>(), "Operation");

  try {
    bpo::store(bpo::command_line_parser(argc, argv).options(opts).allow_unregistered().run(), vm);
    bpo::notify(vm);
  } catch (bpo::error const& e) {
    std::cerr << "Exception from command line processing in " << options->processName() << ": " << e.what() << "\n";
    return process_exit_code::INVALID_ARGUMENT;
  }

  if (vm.count("help") != 0u) {
    std::cerr << opts << std::endl;
    return process_exit_code::HELP;
  }

  if (vm.count(apiliteral::option::operation) == 0u) {
    std::cerr << "Operation option is missing\n";
    std::cerr << opts << std::endl;

    return process_exit_code::INVALID_ARGUMENT;
  }

  auto opname = vm[apiliteral::option::operation].as<std::string>();

  if (opname == apiliteral::operation::addversionalias || opname == apiliteral::operation::rmversionalias) {
    options = std::make_unique<db::configuration::ManageAliasesOperation>(argv[0]);
  } else if (opname == apiliteral::operation::findconfigs) {
    options = std::make_unique<db::configuration::ManageConfigsOperation>(argv[0]);
  } else {
    options = std::make_unique<db::configuration::ManageDocumentOperation>(argv[0]);
  }

  auto result = db::conftool_impl(options, argc, argv);

  if (!result.first) {
    std::cout << "Failed; error message: " << result.second << "\n";
    std::cout << ::debug::current_exception_diagnostic_information();
    return process_exit_code::FAILURE;
  }

  auto returned = std::string{result.second};
  if (!result.first) {
    std::cout << "Test failed; error message: " << result.second << "\n";
    std::cout << ::debug::current_exception_diagnostic_information();
    return process_exit_code::FAILURE;
  }

  if (result.second.empty()) {
    std::cout << "Test failed; error message: result is empty."
              << "\n";
    return process_exit_code::FAILURE;
  }

  auto retdoc = std::string{result.second};

  auto cmpdoc = std::string{};
  db::read_buffer_from_file(cmpdoc, options->resultFileName());

  using cfo::data_format_t;

  if (options->format() == data_format_t::db) {
    auto rdoc = JSONDocument{retdoc};
    auto cdoc = JSONDocument{cmpdoc};
    JSONDocumentBuilder returned{rdoc};
    JSONDocumentBuilder expected{cdoc};

    using namespace artdaq::database::overlay;
    ovl::useCompareMask(DOCUMENT_COMPARE_MUTE_TIMESTAMPS | DOCUMENT_COMPARE_MUTE_OUIDS | DOCUMENT_COMPARE_MUTE_UPDATES);

    auto result = returned == expected;

    if (result.first) {
      return process_exit_code::SUCCESS;
    }

    std::cout << "Error returned!=expected.\n";
    std::cerr << "returned:\n" << returned << "\n";
    std::cerr << "expected:\n" << expected << "\n";
    std::cerr << "error:\n" << result.second << "\n";

    return process_exit_code::FAILURE;
  } else if (options->format() == data_format_t::gui || options->format() == data_format_t::json) {
    auto compare_result = artdaq::database::json::compare_json_objects(retdoc, cmpdoc);
    if (compare_result.first) {
      std::cout << "returned:\n" << retdoc << "\n";

      return process_exit_code::SUCCESS;
    }
    std::cout << "Test failed (expected!=returned); error message: " << compare_result.second << "\n";
  } else if (cmpdoc == retdoc) {
    std::cout << "returned:\n" << retdoc << "\n";

    return process_exit_code::SUCCESS;
  } else if (cmpdoc.pop_back(), cmpdoc == retdoc) {
    std::cout << "returned:\n" << retdoc << "\n";

    return process_exit_code::SUCCESS;
  }

  std::cout << "Test failed (expected!=returned)\n";

  std::cout << "returned:\n" << retdoc << "\n";
  std::cout << "expected:\n" << cmpdoc << "\n";

  auto mismatch = std::mismatch(cmpdoc.begin(), cmpdoc.end(), retdoc.begin());
  std::cout << "File sizes (exp,ret)=(" << std::distance(cmpdoc.begin(), cmpdoc.end()) << ","
            << std::distance(retdoc.begin(), retdoc.end()) << ")\n";

  std::cout << "First mismatch at position " << std::distance(cmpdoc.begin(), mismatch.first) << ", (exp,ret)=(0x"
            << std::hex << static_cast<unsigned int>(*mismatch.first) << ",0x"
            << static_cast<unsigned int>(*mismatch.second) << ")\n";

  auto file_out_name = std::string(db::make_temp_dir())
                           .append("/")
                           .append(argv[0])
                           .append("-")
                           .append(options->operation())
                           .append("-")
                           .append(basename(const_cast<char*>(options->resultFileName().c_str())))
                           .append(".txt");

  db::write_buffer_to_file(retdoc, file_out_name);

  std::cout << "Wrote file:" << file_out_name << "\n";

  return process_exit_code::FAILURE;
} catch (...) {
  std::cout << "Process exited with error: " << ::debug::current_exception_diagnostic_information();
  return process_exit_code::UNCAUGHT_EXCEPTION;
}