#include "artdaq-database/ConfigurationDB/common.h"

#include <boost/exception/diagnostic_information.hpp>
#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/ConfigurationDB/dboperation_exportimport.h"
#include "artdaq-database/ConfigurationDB/dboperation_managedocument.h"
#include "artdaq-database/ConfigurationDB/options_operations.h"
#include "artdaq-database/ConfigurationDB/shared_helper_functions.h"
#include "artdaq-database/DataFormats/shared_literals.h"
#include "artdaq-database/SharedCommon/configuraion_api_literals.h"

#include "artdaq-database/DataFormats/Json/json_common.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "CONF:OpldStr_C"

using namespace artdaq::database::configuration;
namespace dbcfg = artdaq::database::configuration;

namespace db = artdaq::database;
namespace jsonliteral = db::dataformats::literal;
namespace apiliteral = db::configapi::literal;

using artdaq::database::configuration::options::data_format_t;

namespace artdaq {
namespace database {
namespace configuration {
namespace detail {
void export_configuration(ManageDocumentOperation const&, std::string&) noexcept;
void import_configuration(ManageDocumentOperation const&, std::string&) noexcept;
void export_database(ManageDocumentOperation const&, std::string&) noexcept;
void import_database(ManageDocumentOperation const&, std::string&) noexcept;
void export_collection(ManageDocumentOperation const&, std::string&) noexcept;
void import_collection(ManageDocumentOperation const&, std::string&) noexcept;

void configuration_composition(ManageDocumentOperation const&, std::string&);

using artdaq::database::configuration::result_t;

result_t read_document_file(ManageDocumentOperation const& options, std::string const& file_out_name);
result_t write_document_file(ManageDocumentOperation const& options, std::string const& file_src_name);

}  // namespace detail
}  // namespace configuration
}  // namespace database
}  // namespace artdaq

auto make_error_msg = [](auto msg) { return std::string("{\"error\":\"").append(msg).append("\"}"); };

using namespace artdaq::database::result;
using artdaq::database::result_t;

result_t json::export_database(std::string const& query_payload) noexcept {
  try {
    auto options = ManageDocumentOperation{apiliteral::operation::exportdatabase};
    options.readJsonData({query_payload});

    auto returnValue = std::string{};

    detail::export_database(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(boost::current_exception_diagnostic_information());
  }
}

result_t opts::export_database(ManageDocumentOperation const& options) noexcept {
  try {
    auto returnValue = std::string{};

    detail::export_database(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(boost::current_exception_diagnostic_information());
  }
}

result_t json::import_database(std::string const& query_payload) noexcept {
  try {
    auto options = ManageDocumentOperation{apiliteral::operation::importdatabase};
    options.readJsonData({query_payload});

    auto returnValue = std::string{};

    detail::import_database(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(boost::current_exception_diagnostic_information());
  }
}

result_t opts::import_database(ManageDocumentOperation const& options) noexcept {
  try {
    auto returnValue = std::string{};

    detail::import_database(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(boost::current_exception_diagnostic_information());
  }
}

result_t json::export_configuration(std::string const& query_payload) noexcept {
  try {
    auto options = ManageDocumentOperation{apiliteral::operation::exportconfig};
    options.readJsonData({query_payload});

    auto returnValue = std::string{};

    detail::export_configuration(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(boost::current_exception_diagnostic_information());
  }
}

result_t opts::export_configuration(ManageDocumentOperation const& options) noexcept {
  try {
    auto returnValue = std::string{};

    detail::export_configuration(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(boost::current_exception_diagnostic_information());
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
    return Failure(boost::current_exception_diagnostic_information());
  }
}

result_t opts::import_configuration(ManageDocumentOperation const& options) noexcept {
  try {
    auto returnValue = std::string{};

    detail::import_configuration(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(boost::current_exception_diagnostic_information());
  }
}

result_t json::export_collection(std::string const& query_payload) noexcept {
  try {
    auto options = ManageDocumentOperation{apiliteral::operation::exportcollection};
    options.readJsonData({query_payload});

    auto returnValue = std::string{};

    detail::export_collection(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(boost::current_exception_diagnostic_information());
  }
}

result_t opts::export_collection(ManageDocumentOperation const& options) noexcept {
  try {
    auto returnValue = std::string{};

    detail::export_collection(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(boost::current_exception_diagnostic_information());
  }
}

result_t json::import_collection(std::string const& query_payload) noexcept {
  try {
    auto options = ManageDocumentOperation{apiliteral::operation::importcollection};
    options.readJsonData({query_payload});

    auto returnValue = std::string{};

    detail::import_collection(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(boost::current_exception_diagnostic_information());
  }
}

result_t opts::import_collection(ManageDocumentOperation const& options) noexcept {
  try {
    auto returnValue = std::string{};

    detail::import_collection(options, returnValue);
    return Success(returnValue);
  } catch (...) {
    return Failure(boost::current_exception_diagnostic_information());
  }
}

result_t json::write_configuration(std::string const& query_payload, std::string const& conf_tarbzip2_base64) noexcept {
  try {
    if (query_payload.empty()) return Failure(msg_EmptyFilter);
    if (conf_tarbzip2_base64.empty()) return Failure(msg_EmptyDocument);

    if (!system(NULL)) return Failure(msg_SystemCallFailed);

    auto options = ManageDocumentOperation{apiliteral::operation::writeconfiguration};
    options.readJsonData({query_payload});

    TRACE_(10, "write_configuration: operation=<" << options.to_string() << ">");

    confirm(options.operation().compare(apiliteral::operation::writeconfiguration) == 0);

    options.operation(apiliteral::operation::writedocument);

    auto tmp_dir_name = std::string{"/tmp/adb"};
    tmp_dir_name.append(std::to_string(rand() % 9000000 + 1000000));

    auto system_cmd = std::string{"mkdir -p "};
    system_cmd += tmp_dir_name;

    if (0 != system(system_cmd.c_str()))
      throw runtime_error("write_configuration") << "write_configuration: Unable to create a temp directory; "
                                                    "system_cmd="
                                                 << system_cmd;

    auto filename = std::string{tmp_dir_name};
    filename += "/conf.tarbzip2.base64";

    std::ofstream os(filename.c_str());
    std::copy(conf_tarbzip2_base64.begin(), conf_tarbzip2_base64.end(), std::ostream_iterator<char>(os));
    os.close();

    system_cmd = "cat " + filename + " |base64 -d |tar xjf - -C " + tmp_dir_name;

    if (0 != system(system_cmd.c_str()))
      throw runtime_error("write_configuration") << "write_configuration: Unable to unzip configuration files; "
                                                    "system_cmd="
                                                 << system_cmd;

    system_cmd = "rm -f " + filename;

    if (0 != system(system_cmd.c_str()))
      throw runtime_error("write_configuration") << "write_configuration: Unable to delete a temp file ; "
                                                    "system_cmd="
                                                 << system_cmd;

    std::ostringstream oss;

    for (auto const& file_name : list_files(tmp_dir_name)) {
      options.collection(collection_name_from_relative_path(file_name));
      if (file_name.rfind(".fcl") != std::string::npos || file_name.rfind(".fhicl") != std::string::npos)
        options.format(options::data_format_t::fhicl);
      else if (file_name.rfind(".jsn") != std::string::npos || file_name.rfind(".json") != std::string::npos)
        options.format(options::data_format_t::json);
      else if (file_name.rfind(".xml") != std::string::npos)
        options.format(options::data_format_t::xml);
      else
        continue;

      auto result = detail::write_document_file(options, file_name);
      if (!result.first) oss << "Failed to import " << file_name << ", details:" << result.second;
    }

    system_cmd = "rm -rf " + tmp_dir_name;

    if (0 != system(system_cmd.c_str()))
      throw runtime_error("write_configuration") << "write_configuration: Unable to delete a temp directory; "
                                                    "system_cmd="
                                                 << system_cmd;

    auto error_message = std::string{oss.str()};

    if (!error_message.empty()) return Failure(make_error_msg(error_message));

    return Success();
  } catch (...) {
    return Failure(boost::current_exception_diagnostic_information());
  }
}

result_t json::read_configuration(std::string const& query_payload, std::string& conf_tarbzip2_base64) noexcept {
  try {
    if (query_payload.empty()) return Failure(msg_EmptyFilter);
    if (!system(NULL)) return Failure(msg_SystemCallFailed);

    auto options = ManageDocumentOperation{apiliteral::operation::readconfiguration};
    options.readJsonData({query_payload});

    TRACE_(10, "read_configuration: operation=<" << options.to_string() << ">");

    confirm(options.operation().compare(apiliteral::operation::readconfiguration) == 0);

    options.operation(apiliteral::operation::confcomposition);

    auto configuration_list = std::string{};

    detail::configuration_composition(options, configuration_list);

    TRACE_(10, "read_configuration: configuration_list=<" << configuration_list << ">");

    namespace jsn = artdaq::database::json;

    auto resultAST = jsn::object_t{};

    if (!jsn::JsonReader{}.read(configuration_list, resultAST))
      throw runtime_error("read_configuration") << "read_configuration: Unable to read configuration_list";

    auto tmp_dir_name = std::string{"/tmp/adb"};
    tmp_dir_name.append(std::to_string(rand() % 9000000 + 1000000));

    auto system_cmd = std::string{"mkdir -p "};
    system_cmd += tmp_dir_name;

    if (0 != system(system_cmd.c_str()))
      throw runtime_error("read_configuration") << "read_configuration: Unable to zip configuration files; "
                                                   "system_cmd="
                                                << system_cmd;

    auto const& confArray = boost::get<jsn::array_t>(resultAST.at(jsonliteral::search));

    std::ostringstream oss;

    for (auto const& confElement : confArray) {
      auto configuration = std::string{};

      if (!jsn::JsonWriter{}.write(
              boost::get<jsn::object_t>(boost::get<jsn::object_t>(confElement).at(jsonliteral::query)), configuration))
        throw runtime_error("read_configuration") << "read_configuration: Unable to write configuration";

      auto tmpOpts = ManageDocumentOperation{apiliteral::operation::readconfiguration};
      tmpOpts.readJsonData({configuration});

      tmpOpts.format(options::data_format_t::origin);

      TRACE_(10, "read_configuration: looping over configurations operation=<" << tmpOpts.to_string() << ">");

      auto file_name = tmp_dir_name + "/" + relative_path_from_collection_name(tmpOpts.collection());

      auto result = detail::read_document_file(tmpOpts, file_name);

      if (!result.first) oss << "Failed to import " << file_name << ", details:" << result.second;
    }

    auto error_message = std::string{oss.str()};

    if (!error_message.empty()) return Failure(make_error_msg(error_message));

    system_cmd = "tar cjf - " + tmp_dir_name + "/* -C " + tmp_dir_name + " |base64 --wrap=0 > " + tmp_dir_name +
                 "/conf.tar.bzip2.base64";

    if (0 != system(system_cmd.c_str())) {
      throw runtime_error("write_configuration") << "write_configuration: Unable to create a tar.bzip2 file; "
                                                    "system_cmd="
                                                 << system_cmd;
    } else {
      TRACE_(10, "read_configuration: executed command =<" << system_cmd << ">");
    }

    std::ifstream is(std::string{tmp_dir_name + "/conf.tar.bzip2.base64"}.c_str());
    auto database_format = std::string((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
    is.close();

    // database_format.pop_back();

    conf_tarbzip2_base64 = "{\"conf.tar.bzip2.base64\":\"" + database_format + "\"}";
    return Success(conf_tarbzip2_base64);
  } catch (...) {
    return Failure(boost::current_exception_diagnostic_information());
  }
}

void dbcfg::debug::enableExportImport() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);

  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  dbcfg::debug::detail::enableExportImport();

  TRACE_(0, "artdaq::database::configuration::enableExportImport trace_enable");
}