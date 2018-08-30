#include "artdaq-database/ConfigurationDB/common.h"

#include "artdaq-database/ConfigurationDB/dboperation_managealiases.h"

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

#define TRACE_NAME "detail_managealiases.cpp"

namespace db = artdaq::database;
namespace cf = db::configuration;
namespace cftd = cf::debug::detail;
namespace jsonliteral = db::dataformats::literal;

using cf::ManageAliasesOperation;
using cf::options::data_format_t;

using Options = cf::ManageAliasesOperation;

using artdaq::database::docrecord::JSONDocument;
using artdaq::database::docrecord::JSONDocumentBuilder;

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
void write_document(cf::ManageDocumentOperation const&, std::string&);
void read_document(cf::ManageDocumentOperation const&, std::string&);

using provider_call_t = std::vector<JSONDocument> (*)(const Options&, const JSONDocument&);

void find_version_aliases(Options const& options, std::string& configs) {
  confirm(configs.empty());
  confirm(options.operation() == apiliteral::operation::findversionalias);

  TLOG(21) << "find_version_aliases: begin";
  TLOG(21) << "find_version_aliases args options=<" << options << ">";

  validate_dbprovider_name(options.provider());

  auto dispatch_persistence_provider = [](std::string const& name) {
    auto providers = std::map<std::string, provider_call_t>{{apiliteral::provider::mongo, cf::mongo::findVersionAliases},
                                                            {apiliteral::provider::filesystem, cf::filesystem::findVersionAliases},
                                                            {apiliteral::provider::ucon, cf::ucon::findVersionAliases}};

    return providers.at(name);
  };

  auto search_results = dispatch_persistence_provider(options.provider())(options, options.query_filter_to_JsonData());

  auto returnValue = std::string{};
  auto returnValueChanged = bool{false};

  switch (options.format()) {
    default:
    case data_format_t::db:
    case data_format_t::json:
    case data_format_t::unknown:
    case data_format_t::fhicl:
    case data_format_t::xml: {
      throw runtime_error("find_version_aliases") << "Unsupported data format.";
      break;
    }

    case data_format_t::gui: {
      std::ostringstream oss;
      oss << "{ \"search\": [\n";
      for (auto const& search_result : search_results) {
        oss << search_result << ",";
      }

      oss.seekp(-1, oss.cur);
      oss << "] }";
      returnValue = oss.str();
      returnValueChanged = true;
      break;
    }

    case data_format_t::csv: {
      std::ostringstream oss;
      for (auto const& search_result : search_results) {
        oss << search_result.value_as<std::string>(apiliteral::name) << ",";
      }

      returnValue = oss.str();

      if (returnValue.back() == ',') {
        returnValue.pop_back();
      }

      returnValueChanged = true;
      break;
    }
  }

  if (returnValueChanged) {
    configs.swap(returnValue);
  }

  TLOG(21) << "find_version_aliases: end";
}

void add_version_alias(Options const& options, std::string& conf) {
  confirm(conf.empty());
  confirm(options.operation() == apiliteral::operation::addversionalias);

  TLOG(16) << "add_version_alias: begin";
  TLOG(16) << "add_version_alias args options=<" << options << ">";
  TLOG(16) << "add_version_alias args conf=<" << conf << ">";

  validate_dbprovider_name(options.provider());

  auto document = std::string{};

  auto newOptions = cf::ManageDocumentOperation{apiliteral::operation::addversionalias};
  newOptions.readJsonData(options.to_JsonData());
  newOptions.operation(apiliteral::operation::readdocument);
  newOptions.format(data_format_t::db);

  read_document(newOptions, document);
  JSONDocumentBuilder builder{{document}};

  builder.addAlias({options.versionAlias_to_JsonData()});

  newOptions.operation(apiliteral::operation::writedocument);
  newOptions.format(data_format_t::db);

  auto updated = builder.to_string();
  write_document(newOptions, updated);

  newOptions.operation(apiliteral::operation::readdocument);
  newOptions.format(options.format());

  document.clear();
  read_document(newOptions, document);

  conf.swap(document);

  TLOG(16) << "add_version_alias end conf=<" << conf << ">";
}

void remove_version_alias(Options const& options, std::string& conf) {
  confirm(conf.empty());
  confirm(options.operation() == apiliteral::operation::rmversionalias);

  TLOG(15) << "remove_version_alias: begin";
  TLOG(15) << "remove_version_alias args options=<" << options << ">";
  TLOG(15) << "remove_version_alias args conf=<" << conf << ">";

  validate_dbprovider_name(options.provider());

  auto document = std::string{};

  auto newOptions = cf::ManageDocumentOperation{apiliteral::operation::rmversionalias};
  newOptions.readJsonData(options.to_JsonData());
  newOptions.operation(apiliteral::operation::readdocument);

  read_document(newOptions, document);
  JSONDocumentBuilder builder{{document}};

  builder.removeAlias({options.versionAlias_to_JsonData()});

  newOptions.operation(apiliteral::operation::writedocument);
  newOptions.format(data_format_t::db);

  auto updated = builder.to_string();
  write_document(newOptions, updated);

  newOptions.operation(apiliteral::operation::readdocument);
  newOptions.format(options.format());
  newOptions.queryFilter(apiliteral::notprovided);

  document.clear();
  read_document(newOptions, document);

  conf.swap(document);

  TLOG(15) << "remove_version_alias end conf=<" << conf << ">";
}

}  // namespace detail
}  // namespace configuration
}  // namespace database
}  // namespace artdaq

void cftd::ManageAliases() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);

  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TLOG(10) << "artdaq::database::configuration::ManageAliases trace_enable";
}
