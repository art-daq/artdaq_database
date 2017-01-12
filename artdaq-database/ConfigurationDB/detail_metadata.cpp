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

#include <boost/exception/diagnostic_information.hpp>

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

using cf::LoadStoreOperation;
using cf::options::data_format_t;

using Options = cf::LoadStoreOperation;

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

typedef JsonData (*provider_listdatabases_t)(Options const& /*options*/, JsonData const& /*search_filter*/);
typedef JsonData (*provider_listcollections_t)(Options const& /*options*/, JsonData const& /*search_filter*/);
typedef JsonData (*provider_readdbinfo_t)(Options const& /*options*/, JsonData const& /*search_filter*/);

void list_database_names(Options const& options, std::string& configs) {
  confirm(configs.empty());
  confirm(options.operation().compare(apiliteral::operation::listdatabases) == 0);

  TRACE_(11, "list_database_names: begin");
  TRACE_(11, "list_database_names args options=<" << options.to_string() << ">");

  validate_dbprovider_name(options.provider());

  auto dispatch_persistence_provider = [](std::string const& name) {
    auto providers = std::map<std::string, provider_listdatabases_t>{
        {apiliteral::provider::mongo, cf::mongo::listDatabaseNames},
        {apiliteral::provider::filesystem, cf::filesystem::listDatabaseNames},
        {apiliteral::provider::ucond, cf::ucond::listDatabaseNames}};

    return providers.at(name);
  };

  auto search_result =
      dispatch_persistence_provider(options.provider())(options, options.search_filter_to_JsonData().json_buffer);

  auto returnValue = std::string{};
  auto returnValueChanged = bool{false};

  switch (options.dataFormat()) {
    default:
    case data_format_t::db:
    case data_format_t::json:
    case data_format_t::unknown:
    case data_format_t::fhicl:
    case data_format_t::xml: {
      if (!db::json_db_to_gui(search_result.json_buffer, returnValue)) {
        throw runtime_error("list_database_names") << "Unsupported data format.";
      }
      break;
    }

    case data_format_t::gui: {
      returnValue = search_result.json_buffer;
      returnValueChanged = true;
      break;
    }

    case data_format_t::csv: {
      using namespace artdaq::database::json;
      auto reader = JsonReader{};
      object_t results_ast;

      if (!reader.read(search_result.json_buffer, results_ast)) {
        TRACE_(11, "list_database_names() Failed to create an AST from search results JSON.");

        throw runtime_error("list_database_names") << "Failed to create an AST from search results JSON.";
      }

      auto const& results_list = boost::get<array_t>(results_ast.at(jsonliteral::search));

      TRACE_(11, "list_database_names: found " << results_list.size() << " results.");

      std::ostringstream os;

      for (auto const& result_entry : results_list) {
        auto const& buff = boost::get<object_t>(result_entry).at(apiliteral::name);
        auto value = boost::apply_visitor(jsn::tostring_visitor(), buff);

        TRACE_(11, "list_database_names() Found database=<" << value << ">.");

        os << value << ", ";
      }
      returnValue = os.str();
      returnValueChanged = true;
      break;
    }
  }

  if (returnValueChanged) configs.swap(returnValue);

  TRACE_(11, "list_database_names: end");
}
void read_database_info(Options const& options, std::string& filters) {
  confirm(filters.empty());
  confirm(options.operation().compare(apiliteral::operation::readdbinfo) == 0);

  TRACE_(12, "read_database_info: begin");
  TRACE_(11, "read_database_info args options=<" << options.to_string() << ">");

  validate_dbprovider_name(options.provider());

  auto dispatch_persistence_provider = [](std::string const& name) {
    auto providers = std::map<std::string, provider_readdbinfo_t>{
        {apiliteral::provider::mongo, cf::mongo::readDatabaseInfo},
        {apiliteral::provider::filesystem, cf::filesystem::readDatabaseInfo},
        {apiliteral::provider::ucond, cf::ucond::readDatabaseInfo}};

    return providers.at(name);
  };

  auto search_result =
      dispatch_persistence_provider(options.provider())(options, options.search_filter_to_JsonData().json_buffer);

  auto returnValue = std::string{};
  auto returnValueChanged = bool{false};

  switch (options.dataFormat()) {
    default:
    case data_format_t::db:
    case data_format_t::json:
    case data_format_t::unknown:
    case data_format_t::fhicl:
    case data_format_t::xml: {
      if (!db::json_db_to_gui(search_result.json_buffer, returnValue)) {
        throw runtime_error("read_database_info") << "Unsupported data format.";
      }
      break;
    }

    case data_format_t::gui: {
      returnValue = search_result.json_buffer;
      returnValueChanged = true;
      break;
    }
  }

  if (returnValueChanged) filters.swap(returnValue);

  TRACE_(11, "read_database_info: end");
}

void list_collection_names(Options const& options, std::string& collections) {
  confirm(collections.empty());
  confirm(options.operation().compare(apiliteral::operation::listcollections) == 0);

  TRACE_(13, "list_collection_names: begin");
  TRACE_(13, "list_collection_names args search_filter=<" << options.to_string() << ">");

  validate_dbprovider_name(options.provider());

  auto dispatch_persistence_provider = [](std::string const& name) {
    auto providers = std::map<std::string, provider_listcollections_t>{
        {apiliteral::provider::mongo, cf::mongo::listCollectionNames},
        {apiliteral::provider::filesystem, cf::filesystem::listCollectionNames},
        {apiliteral::provider::ucond, cf::ucond::listCollectionNames}};

    return providers.at(name);
  };

  auto search_result =
      dispatch_persistence_provider(options.provider())(options, options.collectionName_to_JsonData().json_buffer);

  auto returnValue = std::string{};
  auto returnValueChanged = bool{false};

  switch (options.dataFormat()) {
    default:
    case data_format_t::db:
    case data_format_t::json:
    case data_format_t::unknown:
    case data_format_t::fhicl:
    case data_format_t::xml: {
      throw runtime_error("list_collection_names") << "Unsupported data format.";
      break;
    }

    case data_format_t::gui: {
      returnValue = search_result.json_buffer;
      returnValueChanged = true;
      break;
    }
    case data_format_t::csv: {
      using namespace artdaq::database::json;
      auto reader = JsonReader{};
      object_t results_ast;

      if (!reader.read(search_result.json_buffer, results_ast)) {
        TRACE_(11, "list_collection_names() Failed to create an AST from search results JSON.");

        throw runtime_error("list_collection_names") << "Failed to create an AST from search results JSON.";
      }

      auto const& results_list = boost::get<array_t>(results_ast.at(jsonliteral::search));

      TRACE_(11, "list_collection_names: found " << results_list.size() << " results.");

      std::ostringstream os;

      for (auto const& result_entry : results_list) {
        auto const& buff = boost::get<object_t>(result_entry).at(apiliteral::name);
        auto value = boost::apply_visitor(jsn::tostring_visitor(), buff);

        TRACE_(11, "list_collection_names() Found collection=<" << value << ">.");

        os << value << ", ";
      }
      returnValue = os.str();

      returnValueChanged = true;
      break;
    }
  }

  if (returnValueChanged) collections.swap(returnValue);

  TRACE_(13, "list_collection_names: end");
}

}  // namespace detail
}  // namespace configuration
}  // namespace database
}  // namespace artdaq

void cftd::enableMetadataOperation() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);

  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TRACE_(0, "artdaq::database::configuration::MetadataOperation trace_enable");
}
