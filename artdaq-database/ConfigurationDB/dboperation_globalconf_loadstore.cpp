#include "artdaq-database/ConfigurationDB/common.h"

#include <boost/exception/diagnostic_information.hpp>
#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/BuildInfo/process_exceptions.h"
#include "artdaq-database/BuildInfo/process_exit_codes.h"
#include "artdaq-database/ConfigurationDB/dboperation_globalconf_loadstore.h"
#include "artdaq-database/ConfigurationDB/dboperation_loadstore.h"
#include "artdaq-database/ConfigurationDB/options_operations.h"
#include "artdaq-database/ConfigurationDB/shared_helper_functions.h"
#include "artdaq-database/ConfigurationDB/shared_literals.h"

#include "artdaq-database/DataFormats/Json/json_common.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "CONF:OpldStr_C"

using namespace artdaq::database::configuration;

using artdaq::database::configuration::options::data_format_t;

namespace artdaq {
namespace database {
namespace configuration {
namespace detail {
void store_configuration(LoadStoreOperation const&, std::string&);
void load_configuration(LoadStoreOperation const&, std::string&);
void build_global_configuration_search_filter(ManageConfigsOperation const&, std::string&);

using artdaq::database::configuration::result_pair_t;
result_pair_t store_configuration_file(LoadStoreOperation const&, std::string const&);
result_pair_t load_configuration_file(LoadStoreOperation const&, std::string const&);
}  // namespace detail
}  // namespace configuration
}  // namespace database
}  // namespace artdaq

auto make_error_msg = [](auto msg) { return std::string("{\"error\":\"").append(msg).append("\"}"); };

result_pair_t json::store_globalconfiguration(std::string const& search_filter, std::string const& conf_tarbzip2_base64) noexcept {
  try {
    if (search_filter.empty()) return std::make_pair(false, make_error_msg(literal::msg::empty_filter));
    if (conf_tarbzip2_base64.empty()) return std::make_pair(false, make_error_msg(literal::msg::empty_document));

    if (!system(NULL)) return std::make_pair(false, make_error_msg(literal::msg::cant_call_system));

    auto options = LoadStoreOperation{literal::operation::globalconfstore};
    options.readJsonData({search_filter});

    TRACE_(10, "store_globalconfiguration: operation=<" << options.to_string() << ">");

    assert(options.operation().compare(literal::operation::globalconfstore) == 0);

    options.operation(literal::operation::store);

    auto tmp_dir_name = std::string{std::tmpnam(nullptr)};

    auto system_cmd = std::string{"mkdir -p "};
    system_cmd += tmp_dir_name;

    if (0 != system(system_cmd.c_str()))
      throw artdaq::database::runtime_exception("store_globalconfiguration") << "store_globalconfiguration: Unable to create a temp directory; "
                                                                                "system_cmd="
                                                                             << system_cmd;

    auto filename = std::string{tmp_dir_name};
    filename += "/conf.tarbzip2.base64";

    std::ofstream os(filename.c_str());
    std::copy(conf_tarbzip2_base64.begin(), conf_tarbzip2_base64.end(), std::ostream_iterator<char>(os));
    os.close();

    system_cmd = "cat " + filename + " |base64 -d |tar xjf - -C " + tmp_dir_name;

    if (0 != system(system_cmd.c_str()))
      throw artdaq::database::runtime_exception("store_globalconfiguration") << "store_globalconfiguration: Unable to unzip configuration files; "
                                                                                "system_cmd="
                                                                             << system_cmd;

    system_cmd = "rm -f " + filename;

    if (0 != system(system_cmd.c_str()))
      throw artdaq::database::runtime_exception("store_globalconfiguration") << "store_globalconfiguration: Unable to delete a temp file ; "
                                                                                "system_cmd="
                                                                             << system_cmd;

    std::ostringstream oss;

    for (auto const& file_name : list_files(tmp_dir_name)) {
      options.collectionName(collection_name_from_relative_path(file_name));
      if (file_name.rfind(".fcl") != std::string::npos || file_name.rfind(".fhicl") != std::string::npos)
        options.dataFormat(options::data_format_t::fhicl);
      else if (file_name.rfind(".jsn") != std::string::npos || file_name.rfind(".json") != std::string::npos)
        options.dataFormat(options::data_format_t::json);
      else if (file_name.rfind(".xml") != std::string::npos)
        options.dataFormat(options::data_format_t::xml);
      else
        continue;

      auto result = detail::store_configuration_file(options, file_name);
      if (!result.first) oss << "Failed to import " << file_name << ", details:" << result.second;
    }

    system_cmd = "rm -rf " + tmp_dir_name;

    if (0 != system(system_cmd.c_str()))
      throw artdaq::database::runtime_exception("store_globalconfiguration") << "store_globalconfiguration: Unable to delete a temp directory; "
                                                                                "system_cmd="
                                                                             << system_cmd;

    auto error_message = std::string{oss.str()};

    if (!error_message.empty()) return result_pair_t{false, make_error_msg(error_message)};

    return result_pair_t{true, make_error_msg("Success")};
  } catch (...) {
    return result_pair_t{false, boost::current_exception_diagnostic_information()};
  }
}

result_pair_t json::load_globalconfiguration(std::string const& search_filter, std::string& conf_tarbzip2_base64) noexcept {
  try {
    if (search_filter.empty()) return std::make_pair(false, make_error_msg(literal::msg::empty_filter));
    if (!system(NULL)) return std::make_pair(false, make_error_msg(literal::msg::cant_call_system));

    auto options = ManageConfigsOperation{literal::operation::globalconfload};
    options.readJsonData({search_filter});

    TRACE_(10, "load_globalconfiguration: operation=<" << options.to_string() << ">");

    assert(options.operation().compare(literal::operation::globalconfload) == 0);

    options.operation(literal::operation::buildfilter);

    auto configuration_list = std::string{};

    detail::build_global_configuration_search_filter(options, configuration_list);

    TRACE_(10, "load_globalconfiguration: configuration_list=<" << configuration_list << ">");

    namespace jsn = artdaq::database::json;

    auto resultAST = jsn::object_t{};

    if (!jsn::JsonReader{}.read(configuration_list, resultAST))
      throw artdaq::database::runtime_exception("load_globalconfiguration") << "load_globalconfiguration: Unable to read configuration_list";

    auto tmp_dir_name = std::string{std::tmpnam(nullptr)};

    auto system_cmd = std::string{"mkdir -p "};
    system_cmd += tmp_dir_name;

    if (0 != system(system_cmd.c_str()))
      throw artdaq::database::runtime_exception("load_globalconfiguration") << "load_globalconfiguration: Unable to zip configuration files; "
                                                                               "system_cmd="
                                                                            << system_cmd;

    auto const& confArray = boost::get<jsn::array_t>(resultAST.at(literal::document::search));

    std::ostringstream oss;

    for (auto const& confElement : confArray) {
      auto configuration = std::string{};

      if (!jsn::JsonWriter{}.write(boost::get<jsn::object_t>(boost::get<jsn::object_t>(confElement).at(literal::document::query)), configuration))
        throw artdaq::database::runtime_exception("load_globalconfiguration") << "load_globalconfiguration: Unable to write configuration";

      auto tmpOpts = LoadStoreOperation{literal::operation::globalconfload};
      tmpOpts.readJsonData({configuration});

      tmpOpts.dataFormat(options::data_format_t::origin);

      TRACE_(10, "load_globalconfiguration: looping over configurations operation=<" << tmpOpts.to_string() << ">");

      auto file_name = tmp_dir_name + "/" + relative_path_from_collection_name(tmpOpts.collectionName());

      auto result = detail::load_configuration_file(tmpOpts, file_name);

      if (!result.first) oss << "Failed to import " << file_name << ", details:" << result.second;
    }

    auto error_message = std::string{oss.str()};

    if (!error_message.empty()) return result_pair_t{false, make_error_msg(error_message)};

    system_cmd = "tar cjf - " + tmp_dir_name + "/* -C " + tmp_dir_name + " |base64 --wrap=0 > " + tmp_dir_name + "/conf.tar.bzip2.base64";

    if (0 != system(system_cmd.c_str())) {
      throw artdaq::database::runtime_exception("store_globalconfiguration") << "store_globalconfiguration: Unable to create a tar.bzip2 file; "
                                                                                "system_cmd="
                                                                             << system_cmd;
    } else {
      TRACE_(10, "load_globalconfiguration: executed command =<" << system_cmd << ">");
    }

    std::ifstream is(std::string{tmp_dir_name + "/conf.tar.bzip2.base64"}.c_str());
    auto database_format = std::string((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
    is.close();

    // database_format.pop_back();

    conf_tarbzip2_base64 = "{\"conf.tar.bzip2.base64\":\"" + database_format + "\"}";
    return result_pair_t{true, conf_tarbzip2_base64};
  } catch (...) {
    return result_pair_t{false, boost::current_exception_diagnostic_information()};
  }
}

namespace artdaq {
namespace database {
namespace configuration {
namespace detail {

result_pair_t load_configuration_file(LoadStoreOperation const& options, std::string const& file_out_name) try {
  assert(!file_out_name.empty());

  TRACE_(11, "load_globalconfiguration: file_name=<" << file_out_name << ">");

  auto test_document = std::string{};

  detail::load_configuration(options, test_document);

  if (!mkdirfile(file_out_name))
    throw artdaq::database::runtime_exception("load_configuration_file") << "load_configuration_file: Unable to create a directory path for "
                                                                            "writing a config file file="
                                                                         << file_out_name;

  std::ofstream os(file_out_name.c_str());
  std::copy(test_document.begin(), test_document.end(), std::ostream_iterator<char>(os));
  os.close();

  return result_pair_t{true, make_error_msg("Success")};
} catch (...) {
  return result_pair_t{false, boost::current_exception_diagnostic_information()};
}

result_pair_t store_configuration_file(LoadStoreOperation const& options, std::string const& file_src_name) try {
  assert(!file_src_name.empty());

  using namespace artdaq::database::configuration::json;

  std::ifstream is(file_src_name);
  auto test_document = std::string((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
  is.close();

  detail::store_configuration(options, test_document);

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

  return result_pair_t{true, make_error_msg("Success")};
} catch (...) {
  return result_pair_t{false, boost::current_exception_diagnostic_information()};
}
}  // namespace detail
}  // namespace configuration
}  // namespace database
}  // namespace artdaq

void debug::enableGlobalConfLoadStoreOperation() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);

  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TRACE_(0,
         "artdaq::database::configuration::GlobalConfLoadStoreOperation "
         "trace_enable");
}