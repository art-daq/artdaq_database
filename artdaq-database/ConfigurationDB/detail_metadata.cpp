#include "artdaq-database/ConfigurationDB/common.h"

#include "artdaq-database/ConfigurationDB/dboperation_metadata.h"

#include "artdaq-database/ConfigurationDB/shared_helper_functions.h"
#include "artdaq-database/DataFormats/shared_literals.h"
#include "artdaq-database/SharedCommon/configuraion_api_literals.h"

#include "artdaq-database/ConfigurationDB/configuration_dbproviders.h"

#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/DataFormats/Json/json_common.h"

#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/JsonDocument/docrecord_literals.h"

#include "artdaq-database/ConfigurationDB/options_operations.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "detail_metadata.cpp"

namespace db = artdaq::database;
namespace cf = db::configuration;
namespace cftd = cf::debug::detail;
namespace jsonliteral = db::dataformats::literal;

using cf::options::data_format_t;

using Options = cf::ManageDocumentOperation;

using artdaq::database::docrecord::JSONDocument;

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

using provider_listdatabases_t = std::vector<JSONDocument> (*)(const Options&, const JSONDocument&);
using provider_listcollections_t = std::vector<JSONDocument> (*)(const Options&, const JSONDocument&);
using provider_readdbinfo_t = JSONDocument (*)(const Options&, const JSONDocument&);

void list_databases(Options const& options, std::string& configs) {
  confirm(configs.empty());
  confirm(options.operation() == apiliteral::operation::listdatabases);

  TLOG(21) << "list_databases: begin";
  TLOG(21) << "list_databases args options=<" << options << ">";

  validate_dbprovider_name(options.provider());
  TLOG(21) << "list_databases: valid";

  auto dispatch_persistence_provider = [](std::string const& name) {
    auto providers = std::map<std::string, provider_listdatabases_t>{{apiliteral::provider::mongo, cf::mongo::listDatabases},
                                                                     {apiliteral::provider::filesystem, cf::filesystem::listDatabases},
                                                                     {apiliteral::provider::ucon, cf::ucon::listDatabases}};

    TLOG(21) << "list_databases: end";

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
      throw runtime_error("list_databases") << "Unsupported data format.";
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

  TLOG(21) << "list_databases: end";
}
void read_dbinfo(Options const& options, std::string& filters) {
  confirm(filters.empty());
  confirm(options.operation() == apiliteral::operation::readdbinfo);

  TLOG(22) << "read_dbinfo: begin";
  TLOG(21) << "read_dbinfo args options=<" << options << ">";

  validate_dbprovider_name(options.provider());

  auto dispatch_persistence_provider = [](std::string const& name) {
    auto providers = std::map<std::string, provider_readdbinfo_t>{{apiliteral::provider::mongo, cf::mongo::readDbInfo},
                                                                  {apiliteral::provider::filesystem, cf::filesystem::readDbInfo},
                                                                  {apiliteral::provider::ucon, cf::ucon::readDbInfo}};

    return providers.at(name);
  };

  auto search_result = dispatch_persistence_provider(options.provider())(options, options.query_filter_to_JsonData());

  auto returnValue = std::string{};
  auto returnValueChanged = bool{false};

  switch (options.format()) {
    default:
    case data_format_t::unknown:
    case data_format_t::fhicl:
    case data_format_t::xml: {
      if (!db::json_db_to_gui(search_result, returnValue)) {
        throw runtime_error("read_dbinfo") << "Unsupported data format.";
      }
      break;
    }

    case data_format_t::db:
    case data_format_t::json:
    case data_format_t::gui: {
      returnValue = search_result;
      returnValueChanged = true;
      break;
    }
  }

  if (returnValueChanged) {
    filters.swap(returnValue);
  }

  TLOG(21) << "read_dbinfo: end";
}

void list_collections(Options const& options, std::string& collections) {
  confirm(collections.empty());
  confirm(options.operation() == apiliteral::operation::listcollections);

  TLOG(23) << "list_collections: begin";
  TLOG(23) << "list_collections args query_payload=<" << options << ">";

  validate_dbprovider_name(options.provider());

  auto dispatch_persistence_provider = [](std::string const& name) {
    auto providers = std::map<std::string, provider_listcollections_t>{{apiliteral::provider::mongo, cf::mongo::listCollections},
                                                                       {apiliteral::provider::filesystem, cf::filesystem::listCollections},
                                                                       {apiliteral::provider::ucon, cf::ucon::listCollections}};

    return providers.at(name);
  };

  auto search_results = dispatch_persistence_provider(options.provider())(options, options.collection_to_JsonData());

  auto returnValue = std::string{};
  auto returnValueChanged = bool{false};

  switch (options.format()) {
    default:
    case data_format_t::db:
    case data_format_t::json:
    case data_format_t::unknown:
    case data_format_t::fhicl:
    case data_format_t::xml: {
      throw runtime_error("list_collections") << "Unsupported data format.";
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
    collections.swap(returnValue);
  }

  TLOG(23) << "list_collections: end";
}

}  // namespace detail
}  // namespace configuration
}  // namespace database
}  // namespace artdaq

void cftd::Metadata() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);

  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TLOG(10) << "artdaq::database::configuration::Metadata trace_enable";
}
