#include "artdaq-database/ConfigurationDB/common.h"

#include "artdaq-database/ConfigurationDB/dboperation_exportimport.h"

#include "artdaq-database/ConfigurationDB/shared_helper_functions.h"
#include "artdaq-database/DataFormats/shared_literals.h"
#include "artdaq-database/SharedCommon/configuraion_api_literals.h"

#include "artdaq-database/ConfigurationDB/configuration_dbproviders.h"

#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/DataFormats/Json/json_common.h"
#include "artdaq-database/DataFormats/shared_literals.h"

#include "artdaq-database/ConfigurationDB/options_operations.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "CONF:FndCfD_C"

namespace db = artdaq::database;
namespace cf = db::configuration;
namespace cfj = db::configuration::json;

namespace cftd = cf::debug::detail;
namespace jsonliteral = db::dataformats::literal;
namespace apiliteral = db::configapi::literal;

using cf::ManageDocumentOperation;
using cf::options::data_format_t;

using Options = cf::ManageDocumentOperation;

using artdaq::database::configuration::result_t;

using artdaq::database::basictypes::JsonData;
using artdaq::database::basictypes::FhiclData;
using artdaq::database::docrecord::JSONDocument;
using artdaq::database::docrecord::JSONDocumentBuilder;

namespace artdaq {
namespace database {
bool json_db_to_gui(std::string const&, std::string&);
bool json_gui_to_db(std::string const&, std::string&);
}
}

namespace artdaq {
namespace database {
namespace configuration {
namespace detail {
result_t read_document_file(ManageDocumentOperation const& options, std::string const& file_out_name);
result_t write_document_file(ManageDocumentOperation const& options, std::string const& file_src_name);
void write_document(ManageDocumentOperation const&, std::string&);
void read_document(ManageDocumentOperation const&, std::string&);

void export_configuration(ManageDocumentOperation const&, std::string&);
void import_configuration(ManageDocumentOperation const&, std::string&);
void export_database(ManageDocumentOperation const&, std::string&);
void import_database(ManageDocumentOperation const&, std::string&);
void export_collection(ManageDocumentOperation const&, std::string&);
void import_collection(ManageDocumentOperation const&, std::string&);

}  // namespace detail
}  // namespace configuration
}  // namespace database
}  // namespace artdaq

namespace cfd = db::configuration::detail;

result_t cfd::read_document_file(ManageDocumentOperation const& options, std::string const& file_out_name) try {
  confirm(!file_out_name.empty());

  TRACE_(11, "read_configuration: file_name=<" << file_out_name << ">");

  auto test_document = std::string{};

  detail::read_document(options, test_document);

  if (!mkdirfile(file_out_name))
    throw runtime_error("read_document_file") << "read_document_file: Unable to create a directory path for "
                                                 "writing a config file file="
                                              << file_out_name;

  std::ofstream os(file_out_name.c_str());
  std::copy(test_document.begin(), test_document.end(), std::ostream_iterator<char>(os));
  os.close();

  return Success();
} catch (...) {
  return Failure(boost::current_exception_diagnostic_information());
}

result_t cfd::write_document_file(ManageDocumentOperation const& options, std::string const& file_src_name) try {
  confirm(!file_src_name.empty());

  using namespace artdaq::database::configuration::json;

  std::ifstream is(file_src_name);
  auto test_document = std::string((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
  is.close();

  detail::write_document(options, test_document);

#ifdef _WRITE_DEBUG_FILE_
  std::cout << "Returned buffer:\n" << result.second << "\n";

  auto file_out_name = std::string(artdaq::database::mkdir(tmpdir))
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
#endif  //_WRITE_DEBUG_FILE_

  return Success();
} catch (...) {
  return Failure(boost::current_exception_diagnostic_information());
}

void cfd::export_configuration(ManageDocumentOperation const&, std::string&) {
  throw runtime_error("export_configuration") << "export_configuration: is not implemented";
}

void cfd::import_configuration(ManageDocumentOperation const&, std::string&) {
  throw runtime_error("import_configuration") << "import_configuration: is not implemented";
}

void cfd::export_database(ManageDocumentOperation const&, std::string&) {
  throw runtime_error("export_database") << "export_database: is not implemented";
}

void cfd::import_database(ManageDocumentOperation const&, std::string&) {
  throw runtime_error("import_database") << "import_database: is not implemented";
}

void cfd::export_collection(ManageDocumentOperation const&, std::string&) {
  throw runtime_error("export_collection") << "export_collection: is not implemented";
}

void cfd::import_collection(ManageDocumentOperation const&, std::string&) {
  throw runtime_error("import_collection") << "import_collection: is not implemented";
}

void cftd::ExportImport() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);

  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TRACE_(0, "artdaq::database::configuration::ExportImport trace_enable");
}
