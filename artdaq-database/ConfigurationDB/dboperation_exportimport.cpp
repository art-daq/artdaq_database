#include "artdaq-database/ConfigurationDB/common.h"

#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/ConfigurationDB/dboperation_exportimport.h"
#include "artdaq-database/ConfigurationDB/dboperation_managedocument.h"
#include "artdaq-database/ConfigurationDB/dboperation_metadata.h"
#include "artdaq-database/ConfigurationDB/options_operations.h"
#include "artdaq-database/ConfigurationDB/shared_helper_functions.h"
#include "artdaq-database/DataFormats/shared_literals.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/SharedCommon/configuraion_api_literals.h"

#include "artdaq-database/DataFormats/Json/json_common.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "dboperation_exportimport.cpp"

using namespace artdaq::database::configuration;
namespace dbcfg = artdaq::database::configuration;

namespace db = artdaq::database;
namespace jsonliteral = db::dataformats::literal;

using artdaq::database::configuration::options::data_format_t;
using artdaq::database::docrecord::JSONDocumentBuilder;
using artdaq::database::docrecord::JSONDocument;

namespace artdaq {
namespace database {
namespace configuration {
namespace detail {
void export_configuration(ManageDocumentOperation const&, std::string&);
void import_configuration(ManageDocumentOperation const&, std::string&);

void export_database(ManageDocumentOperation const&, std::string&);
void import_database(ManageDocumentOperation const&, std::string&);

void import_collection(ManageDocumentOperation const&, std::string&);
void export_collection(ManageDocumentOperation const&, std::string&);

void configuration_composition(ManageDocumentOperation const&, std::string&);

void write_documents(ManageDocumentOperation const&, std::vector<JSONDocument> const&);
void read_documents(ManageDocumentOperation const&, std::vector<JSONDocument>&);

using artdaq::database::configuration::result_t;

result_t read_document_file(ManageDocumentOperation const& options, std::string const& file_out_name);
result_t write_document_file(ManageDocumentOperation const& options, std::string const& file_src_name);

}  // namespace detail
}  // namespace configuration
}  // namespace database
}  // namespace artdaq

auto make_error_msg = [](auto msg) { return std::string(R"({"error":")").append(msg).append("\"}"); };

using namespace artdaq::database::result;
using artdaq::database::result_t;

result_t opts::export_database(ManageDocumentOperation const& options) noexcept {
  try {
    return json::export_database(options.to_JsonData());
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}

result_t json::export_database(std::string const& query_payload) noexcept {
  try {
    if (query_payload.empty()) {
      return Failure(msg_EmptyFilter);
    }

    if (system(nullptr) == 0) {
      return Failure(msg_SystemCallFailed);
    }

    auto options = ManageDocumentOperation{apiliteral::operation::exportdatabase};
    options.readJsonData({query_payload});

    TLOG(20) << "export_database: operation=<" << options << ">";

    confirm(options.operation() == apiliteral::operation::exportdatabase);

    options.format(options::data_format_t::csv);
    options.operation(apiliteral::operation::listcollections);

    auto result = opts::list_collections(options);

    if (!result.first) {
      return Failure(result.second);
    }

    auto tmp = result.second;
    std::replace(tmp.begin(), tmp.end(), ',', ' ');
    std::istringstream iss(tmp);
    std::vector<std::string> collections{std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>{}};
    std::sort(collections.begin(), collections.end());

    options.operation(apiliteral::operation::exportcollection);
    options.format(options::data_format_t::db);

    auto dirname = options.sourceFileName();

    std::ostringstream oss, res, err;

    auto noerror_pos = err.tellp();

    res << "{" << quoted_(apiliteral::option::result) << ":[";

    for (auto const& collection : collections) {
      oss.str("");
      oss.clear();
      options.collection(collection);
      oss << dirname << "/" << collection << apiliteral::dbexport_extension;
      options.sourceFileName(oss.str());

      TLOG(20) << "export_database: exporting collection=" << collection << " to file=" << options.sourceFileName();

      auto result = opts::export_collection(options);

      if (!result.first) {
        err << result.second << ", ";
      }

      res << quoted_(options.sourceFileName()) << ",";
    }
    res.seekp(-1, res.cur);
    res << "]}";

    if (err.tellp() != noerror_pos) {
      return Failure(err);
    }

    return Success(res);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}

result_t opts::import_database(ManageDocumentOperation const& options) noexcept {
  try {
    return json::import_database(options.to_JsonData());
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}

result_t json::import_database(std::string const& query_payload) noexcept {
  try {
    if (query_payload.empty()) {
      return Failure(msg_EmptyFilter);
    }

    if (system(nullptr) == 0) {
      return Failure(msg_SystemCallFailed);
    }

    auto options = ManageDocumentOperation{apiliteral::operation::importdatabase};
    options.readJsonData({query_payload});

    TLOG(20) << "import_database: operation=<" << options << ">";

    confirm(options.operation() == apiliteral::operation::importdatabase);

    options.operation(apiliteral::operation::importcollection);
    options.format(options::data_format_t::db);

    auto dirname = options.sourceFileName();

    std::ostringstream oss, res, err;

    auto noerror_pos = err.tellp();

    res << "{" << quoted_(apiliteral::option::source) << ":[";

    for (auto const& file_name : list_files(dirname)) {
      auto collection = std::string{};

      if (!db::extract_collectionname_from_filename(file_name, collection)) {
        continue;
      }

      oss.str("");
      oss.clear();
      options.collection(collection);
      oss << dirname << "/" << collection << apiliteral::dbexport_extension;
      options.sourceFileName(oss.str());

      TLOG(20) << "import_database: importing collection=" << collection << " to file=" << options.sourceFileName();

      auto result = opts::import_collection(options);

      if (!result.first) {
        err << result.second << ", ";
      }

      res << quoted_(options.sourceFileName()) << ",";
    }
    res.seekp(-1, res.cur);
    res << "]}";

    if (err.tellp() != noerror_pos) {
      return Failure(err);
    }

    return Success(res);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}

result_t opts::export_configuration(ManageDocumentOperation const& options) noexcept {
  try {
    auto returnValue = std::string{};

    detail::export_configuration(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}

result_t json::export_configuration(std::string const& query_payload) noexcept {
  try {
    auto options = ManageDocumentOperation{apiliteral::operation::importconfig};
    options.readJsonData({query_payload});

    auto returnValue = std::string{};

    detail::export_configuration(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}

result_t json::import_configuration(std::string const& query_payload) noexcept {
  try {
    auto options = ManageDocumentOperation{apiliteral::operation::importconfig};
    options.readJsonData({query_payload});

    auto returnValue = std::string{};

    detail::import_configuration(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}

result_t opts::import_configuration(ManageDocumentOperation const& options) noexcept {
  try {
    auto returnValue = std::string{};

    detail::import_configuration(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}

result_t json::write_configuration(std::string const& query_payload, std::string const& tarbzip2_base64) noexcept {
  try {
    if (query_payload.empty()) {
      return Failure(msg_EmptyFilter);
    }
    if (tarbzip2_base64.empty()) {
      return Failure(msg_EmptyDocument);
    }

    if (system(nullptr) == 0) {
      return Failure(msg_SystemCallFailed);
    }

    auto options = ManageDocumentOperation{apiliteral::operation::writeconfiguration};
    options.readJsonData({query_payload});

    TLOG(20) << "write_configuration: operation=<" << options << ">";

    confirm(options.operation() == apiliteral::operation::writeconfiguration);

    options.operation(apiliteral::operation::writedocument);

    auto tmp_dir_name = db::make_temp_dir();

    auto file_name = tmp_dir_name + "/userdata" + apiliteral::dbexport_extension;

    if (!db::write_buffer_to_file(tarbzip2_base64, file_name)) {
      return Failure(make_error_msg("Unable to write userdata"));
    }

    db::tarbzip2base64_to_dir(file_name, tmp_dir_name);

    std::ostringstream oss;

    for (auto const& file_name : list_files(tmp_dir_name)) {
      options.collection(collection_name_from_relative_path(file_name));
      if (file_name.rfind(".fcl") != std::string::npos || file_name.rfind(".fhicl") != std::string::npos) {
        options.format(options::data_format_t::fhicl);
      } else if (file_name.rfind(".jsn") != std::string::npos || file_name.rfind(".json") != std::string::npos) {
        options.format(options::data_format_t::json);
      } else if (file_name.rfind(".xml") != std::string::npos) {
        options.format(options::data_format_t::xml);
      } else {
        continue;
      }

      auto result = detail::write_document_file(options, file_name);
      if (!result.first) {
        oss << "Failed to import " << file_name << ", details:" << result.second;
      }
    }

    db::delete_temp_dir(tmp_dir_name);

    auto error_message = std::string{oss.str()};

    if (!error_message.empty()) {
      return Failure(make_error_msg(error_message));
    }

    return Success();
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}

result_t json::read_configuration(std::string const& query_payload, std::string& tarbzip2_base64) noexcept {
  try {
    if (query_payload.empty()) {
      return Failure(msg_EmptyFilter);
    }
    if (system(nullptr) == 0) {
      return Failure(msg_SystemCallFailed);
    }
    if (!tarbzip2_base64.empty()) {
      return Failure(make_error_msg("Invalid tarbzip2_base64 stream; buffer is not empty."));
    }

    auto options = ManageDocumentOperation{apiliteral::operation::readconfiguration};
    options.readJsonData({query_payload});

    TLOG(20) << "read_configuration: operation=<" << options << ">";

    confirm(options.operation() == apiliteral::operation::readconfiguration);

    options.operation(apiliteral::operation::confcomposition);

    auto configuration_list = std::string{};

    detail::configuration_composition(options, configuration_list);

    TLOG(20) << "read_configuration: configuration_list=<" << configuration_list << ">";

    namespace jsn = artdaq::database::json;

    auto resultAST = jsn::object_t{};

    if (!jsn::JsonReader{}.read(configuration_list, resultAST)) {
      throw runtime_error("read_configuration") << "read_configuration: Unable to read configuration_list";
    }

    auto tmp_dir_name = db::make_temp_dir();

    auto const& confArray = boost::get<jsn::array_t>(resultAST.at(jsonliteral::search));

    std::ostringstream oss;

    for (auto const& confElement : confArray) {
      auto configuration = std::string{};

      if (!jsn::JsonWriter{}.write(
              boost::get<jsn::object_t>(boost::get<jsn::object_t>(confElement).at(jsonliteral::query)),
              configuration)) {
        throw runtime_error("read_configuration") << "read_configuration: Unable to write configuration";
      }

      auto tmpOpts = ManageDocumentOperation{apiliteral::operation::readconfiguration};
      tmpOpts.readJsonData({configuration});

      tmpOpts.format(options::data_format_t::origin);

      TLOG(20) << "read_configuration: looping over configurations operation=<" << tmpOpts.to_string() << ">";

      auto file_name = tmp_dir_name + "/" + relative_path_from_collection_name(tmpOpts.collection());

      auto result = detail::read_document_file(tmpOpts, file_name);

      if (!result.first) {
        oss << "Failed to import " << file_name << ", details:" << result.second;
      }
    }

    auto error_message = std::string{oss.str()};

    if (!error_message.empty()) {
      return Failure(make_error_msg(error_message));
    }

    auto buffer = std::string{};

    oss.str("");
    oss.clear();

    auto file_name = tmp_dir_name + "/userdata" + apiliteral::dbexport_extension;

    db::dir_to_tarbzip2base64(tmp_dir_name, file_name);

    if (!db::read_buffer_from_file(buffer, file_name)) {
      return Failure(make_error_msg("Unable to read userdata"));
    }

    oss << R"({"collection.tar.bzip2.base64":")" << buffer << "\"}";

    db::delete_temp_dir(tmp_dir_name);

    // database_format.pop_back();

    tarbzip2_base64 = oss.str();

    TLOG(20) << "read_configuration: end";

    return Success(tarbzip2_base64);
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}

result_t opts::export_collection(ManageDocumentOperation const& options) noexcept {
  try {
    return json::export_collection(options.to_JsonData());
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}

result_t json::export_collection(std::string const& query_payload) noexcept {
  try {
    if (query_payload.empty()) {
      return Failure(msg_EmptyFilter);
    }

    if (system(nullptr) == 0) {
      return Failure(msg_SystemCallFailed);
    }

    auto options = ManageDocumentOperation{apiliteral::operation::exportcollection};
    options.readJsonData({query_payload});

    TLOG(20) << "export_collection: operation=<" << options << ">";

    confirm(options.operation() == apiliteral::operation::exportcollection);

    options.operation(apiliteral::operation::readdocument);
    options.format(options::data_format_t::db);

    using provider_call_t = std::vector<JsonData> (*)(const ManageDocumentOperation&, const JsonData&);

    auto document_list = std::vector<JSONDocument>{};

    detail::read_documents(options, document_list);

    if (document_list.empty()) {
      return Failure(make_error_msg(options.collection() + " is empty."));
    }

    TLOG(20) << "export_collection: found " << document_list.size() << "documents.";

    auto tmp_dir_name = db::make_temp_dir();

    auto file_name = tmp_dir_name + "/" + options.collection();

    for (auto const& document : document_list) {
      std::ostringstream oss;
      oss << file_name << "/" << JSONDocumentBuilder{document}.getObjectOUID() << ".json";

      if (!db::write_buffer_to_file(document.to_string(), oss.str())) {
        throw runtime_error("export_collection")
            << "export_collection: Unable to write a json file; file=" << oss.str();
      }
    }

    std::ostringstream oss;
    oss << "{" << quoted_(apiliteral::option::result) << ":[";
    oss << quoted_(db::dir_to_tarbzip2base64(tmp_dir_name, options.sourceFileName())) << "]}";

    db::delete_temp_dir(tmp_dir_name);

    TLOG(13) << "export_collection: end";

    return Success(oss.str());
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}

result_t opts::import_collection(ManageDocumentOperation const& options) noexcept {
  try {
    return json::import_collection(options.to_JsonData());
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}

result_t json::import_collection(std::string const& query_payload) noexcept {
  try {
    if (query_payload.empty()) {
      return Failure(msg_EmptyFilter);
    }

    if (system(nullptr) == 0) {
      return Failure(msg_SystemCallFailed);
    }

    auto options = ManageDocumentOperation{apiliteral::operation::importcollection};
    options.readJsonData({query_payload});

    TLOG(20) << "import_collection: operation=<" << options << ">";

    confirm(options.operation() == apiliteral::operation::importcollection);

    options.operation(apiliteral::operation::writedocument);
    options.format(options::data_format_t::db);

    auto tmp_dir_name = db::make_temp_dir();

    db::tarbzip2base64_to_dir(options.sourceFileName(), tmp_dir_name);

    std::ostringstream oss;

    auto document_list = std::vector<JSONDocument>{};

    for (auto const& file_name : list_files(tmp_dir_name)) {
      auto buffer = std::string();

      if (!db::read_buffer_from_file(buffer, file_name)) {
        oss << "Failed to import " << file_name << "\n";
      }

      document_list.emplace_back(JSONDocumentBuilder(buffer).extract());
    }

    db::delete_temp_dir(tmp_dir_name);

    auto error_message = std::string{oss.str()};

    if (!error_message.empty()) {
      return Failure(make_error_msg(error_message));
    }

    if (document_list.empty()) {
      return Failure(make_error_msg(options.collection() + " is empty."));
    }

    detail::write_documents(options, document_list);

    oss.str("");
    oss.clear();
    oss << "{" << quoted_(apiliteral::option::source) << ":[";
    oss << quoted_(options.sourceFileName()) << "]}";

    TLOG(13) << "import_collection: end";

    return Success(oss.str());
  } catch (...) {
    return Failure(::debug::current_exception_diagnostic_information());
  }
}

void dbcfg::debug::ExportImport() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);

  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  dbcfg::debug::detail::ExportImport();

  TLOG(10) << "artdaq::database::configuration::ExportImport trace_enable";
}