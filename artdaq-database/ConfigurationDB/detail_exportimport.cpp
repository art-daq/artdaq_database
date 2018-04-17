#include "artdaq-database/ConfigurationDB/common.h"

#include "artdaq-database/ConfigurationDB/dboperation_exportimport.h"

#include "artdaq-database/ConfigurationDB/shared_helper_functions.h"
#include "artdaq-database/DataFormats/shared_literals.h"
#include "artdaq-database/SharedCommon/configuraion_api_literals.h"

#include "artdaq-database/ConfigurationDB/configuration_dbproviders.h"

#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/DataFormats/Json/json_common.h"
#include "artdaq-database/DataFormats/shared_literals.h"

#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"

#include "artdaq-database/ConfigurationDB/options_operations.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "CONF:FndCfD_C"

namespace db = artdaq::database;
namespace cf = db::configuration;

namespace cftd = cf::debug::detail;

using cf::ManageDocumentOperation;

using Options = cf::ManageDocumentOperation;

using artdaq::database::configuration::result_t;

namespace artdaq {
namespace database {
bool json_db_to_gui(std::string const&, std::string&);
bool json_gui_to_db(std::string const&, std::string&);
}  // namespace database
}  // namespace artdaq

namespace artdaq {
namespace database {
namespace configuration {
namespace detail {
result_t read_document_file(ManageDocumentOperation const& options, std::string const& file_out_name);
result_t write_document_file(ManageDocumentOperation const& options, std::string const& file_src_name);
void write_document(ManageDocumentOperation const&, std::string&);
void read_document(ManageDocumentOperation const&, std::string&);

void export_configuration(ManageDocumentOperation const& /*unused*/, std::string& /*unused*/);
void import_configuration(ManageDocumentOperation const& /*unused*/, std::string& /*unused*/);

}  // namespace detail
}  // namespace configuration
}  // namespace database
}  // namespace artdaq

namespace cfd = db::configuration::detail;

result_t cfd::read_document_file(ManageDocumentOperation const& options, std::string const& file_out_name) try {
  confirm(!file_out_name.empty());

  TLOG(21) << "read_configuration: file_name=<" << file_out_name << ">";

  auto test_document = std::string{};

  detail::read_document(options, test_document);

  if (!write_buffer_to_file(test_document, file_out_name)) {
    return Failure();
  }

  return Success();
} catch (...) {
  return Failure(::debug::current_exception_diagnostic_information());
}

result_t cfd::write_document_file(ManageDocumentOperation const& options, std::string const& file_src_name) try {
  confirm(!file_src_name.empty());

  using namespace artdaq::database::configuration::json;

  auto test_document = std::string{};

  if (!read_buffer_from_file(test_document, file_src_name)) {
    return Failure();
  }

  detail::write_document(options, test_document);

#ifdef _WRITE_DEBUG_FILE_
  std::cout << "Returned buffer:\n" << result.second << "\n";

  auto file_out_name = std::string(artdaq::database::mkdir(tmpdir))
                           .append("/")
                           .append(option::appname)
                           .append("-")
                           .append(options.operation())
                           .append("-")
                           .append(basename((char*)file_src_name.c_str()))
                           .append(".txt");

  db::write_buffer_to_file(result, file_out_name);

  std::cout << "Wrote file:" << file_out_name << "\n";
#endif  //_WRITE_DEBUG_FILE_

  return Success();
} catch (...) {
  return Failure(::debug::current_exception_diagnostic_information());
}

void cfd::export_configuration(ManageDocumentOperation const& /*unused*/, std::string& /*unused*/) {
  throw runtime_error("export_configuration") << "export_configuration: is not implemented";
}

void cfd::import_configuration(ManageDocumentOperation const& /*unused*/, std::string& /*unused*/) {
  throw runtime_error("import_configuration") << "import_configuration: is not implemented";
}

void cftd::ExportImport() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);

  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TLOG(10) << "artdaq::database::configuration::ExportImport trace_enable";
}
