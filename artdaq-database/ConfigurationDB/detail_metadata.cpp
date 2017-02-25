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

#define TRACE_NAME "CONF:FndCfD_C"

namespace db = artdaq::database;
namespace cf = db::configuration;
namespace cftd = cf::debug::detail;
namespace jsonliteral = db::dataformats::literal;
namespace apiliteral = db::configapi::literal;

using cf::options::data_format_t;

using Options = cf::ManageDocumentOperation;

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

typedef JsonData (*provider_listdatabases_t)(Options const& /*options*/, JsonData const& /*task_payload*/);
typedef JsonData (*provider_listcollections_t)(Options const& /*options*/, JsonData const& /*task_payload*/);
typedef JsonData (*provider_readdbinfo_t)(Options const& /*options*/, JsonData const& /*task_payload*/);

void list_databases(Options const& options, std::string& configs) {
  confirm(configs.empty());
  confirm(options.operation().compare(apiliteral::operation::listdatabases) == 0);

  TRACE_(11, "list_databases: begin");
  TRACE_(11, "list_databases args options=<" << options << ">");

  validate_dbprovider_name(options.provider());

  auto dispatch_persistence_provider = [](std::string const& name) {
    auto providers = std::map<std::string, provider_listdatabases_t>{
        {apiliteral::provider::mongo, cf::mongo::listDatabases},
        {apiliteral::provider::filesystem, cf::filesystem::listDatabases},
        {apiliteral::provider::ucon, cf::ucon::listDatabases}};

    return providers.at(name);
  };

  auto search_result =
      dispatch_persistence_provider(options.provider())(options, options.query_filter_to_JsonData());

  auto returnValue = std::string{};
  auto returnValueChanged = bool{false};

  switch (options.format()) {
    default:
    case data_format_t::db:
    case data_format_t::json:
    case data_format_t::unknown:
    case data_format_t::fhicl:
    case data_format_t::xml: {
      if (!db::json_db_to_gui(search_result, returnValue)) {
        throw runtime_error("list_databases") << "Unsupported data format.";
      }
      break;
    }

    case data_format_t::gui: {
      returnValue = search_result;
      returnValueChanged = true;
      break;
    }

    case data_format_t::csv: {
      using namespace artdaq::database::json;
      auto reader = JsonReader{};
      object_t results_ast;

      if (!reader.read(search_result, results_ast)) {
        TRACE_(11, "list_databases() Failed to create an AST from search results JSON.");

        throw runtime_error("list_databases") << "Failed to create an AST from search results JSON.";
      }

      auto const& results_list = boost::get<array_t>(results_ast.at(jsonliteral::search));

      TRACE_(11, "list_databases: found " << results_list.size() << " results.");

      std::ostringstream os;

      for (auto const& result_entry : results_list) {
        auto const& buff = boost::get<object_t>(result_entry).at(apiliteral::name);
        auto value = boost::apply_visitor(jsn::tostring_visitor(), buff);

        TRACE_(11, "list_databases() Found database=<" << value << ">.");

        os << value << ", ";
      }
      returnValue = os.str();
      returnValueChanged = true;
      break;
    }
  }

  if (returnValueChanged) configs.swap(returnValue);

  TRACE_(11, "list_databases: end");
}
void read_dbinfo(Options const& options, std::string& filters) {
  confirm(filters.empty());
  confirm(options.operation().compare(apiliteral::operation::readdbinfo) == 0);

  TRACE_(12, "read_dbinfo: begin");
  TRACE_(11, "read_dbinfo args options=<" << options << ">");

  validate_dbprovider_name(options.provider());

  auto dispatch_persistence_provider = [](std::string const& name) {
    auto providers =
        std::map<std::string, provider_readdbinfo_t>{{apiliteral::provider::mongo, cf::mongo::readDbInfo},
                                                     {apiliteral::provider::filesystem, cf::filesystem::readDbInfo},
                                                     {apiliteral::provider::ucon, cf::ucon::readDbInfo}};

    return providers.at(name);
  };

  auto search_result =
      dispatch_persistence_provider(options.provider())(options, options.query_filter_to_JsonData());

  auto returnValue = std::string{};
  auto returnValueChanged = bool{false};

  switch (options.format()) {
    default:
    case data_format_t::db:
    case data_format_t::json:
    case data_format_t::unknown:
    case data_format_t::fhicl:
    case data_format_t::xml: {
      if (!db::json_db_to_gui(search_result, returnValue)) {
        throw runtime_error("read_dbinfo") << "Unsupported data format.";
      }
      break;
    }

    case data_format_t::gui: {
      returnValue = search_result;
      returnValueChanged = true;
      break;
    }
  }

  if (returnValueChanged) filters.swap(returnValue);

  TRACE_(11, "read_dbinfo: end");
}

void list_collections(Options const& options, std::string& collections) {
  confirm(collections.empty());
  confirm(options.operation().compare(apiliteral::operation::listcollections) == 0);

  TRACE_(13, "list_collections: begin");
  TRACE_(13, "list_collections args query_payload=<" << options << ">");

  validate_dbprovider_name(options.provider());

  auto dispatch_persistence_provider = [](std::string const& name) {
    auto providers = std::map<std::string, provider_listcollections_t>{
        {apiliteral::provider::mongo, cf::mongo::listCollections},
        {apiliteral::provider::filesystem, cf::filesystem::listCollections},
        {apiliteral::provider::ucon, cf::ucon::listCollections}};

    return providers.at(name);
  };

  auto search_result =
      dispatch_persistence_provider(options.provider())(options, options.collection_to_JsonData());

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
      returnValue = search_result;
      returnValueChanged = true;
      break;
    }
    case data_format_t::csv: {
      using namespace artdaq::database::json;
      auto reader = JsonReader{};
      object_t results_ast;

      if (!reader.read(search_result, results_ast)) {
        TRACE_(11, "list_collections() Failed to create an AST from search results JSON.");

        throw runtime_error("list_collections") << "Failed to create an AST from search results JSON.";
      }

      auto const& results_list = boost::get<array_t>(results_ast.at(jsonliteral::search));

      TRACE_(11, "list_collections: found " << results_list.size() << " results.");

      std::ostringstream os;

      for (auto const& result_entry : results_list) {
        auto const& buff = boost::get<object_t>(result_entry).at(apiliteral::name);
        auto value = boost::apply_visitor(jsn::tostring_visitor(), buff);

        TRACE_(11, "list_collections() Found collection=<" << value << ">.");

        os << value << ", ";
      }
      returnValue = os.str();

      returnValueChanged = true;
      break;
    }
  }

  if (returnValueChanged) collections.swap(returnValue);

  TRACE_(13, "list_collections: end");
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

  TRACE_(0, "artdaq::database::configuration::Metadata trace_enable");
}
