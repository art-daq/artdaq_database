#include "artdaq-database/ConfigurationDB/common.h"

#include "artdaq-database/ConfigurationDB/dboperation_metadata.h"

#include "artdaq-database/ConfigurationDB/shared_helper_functions.h"
#include "artdaq-database/ConfigurationDB/shared_literals.h"

#include "artdaq-database/ConfigurationDB/configuration_dbproviders.h"

#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/DataFormats/Json/json_common.h"
#include "artdaq-database/DataFormats/shared_literals.h"

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
namespace cfl = cf::literal;
namespace cflo = cfl::operation;
namespace cflp = cfl::provider;
namespace cftd = cf::debug::detail;

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
  assert(configs.empty());
  assert(options.operation().compare(cflo::listdatabases) == 0);

  TRACE_(11, "list_database_names: begin");
  TRACE_(11, "list_database_names args options=<" << options.to_string() << ">");

  validate_dbprovider_name(options.provider());

  auto dispatch_persistence_provider = [](std::string const& name) {
    auto providers = std::map<std::string, provider_listdatabases_t>{{cflp::mongo, cf::mongo::listDatabaseNames},
                                                                     {cflp::filesystem, cf::filesystem::listDatabaseNames},
                                                                     {cflp::ucond, cf::ucond::listDatabaseNames}};

    return providers.at(name);
  };

  auto search_result = dispatch_persistence_provider(options.provider())(options, options.search_filter_to_JsonData().json_buffer);

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
        throw cet::exception("list_database_names") << "Unsupported data format.";
      }
      break;
    }

    case data_format_t::gui: {
      returnValue = search_result.json_buffer;
      returnValueChanged = true;
      break;
    }

    case data_format_t::console: {
      using namespace artdaq::database::json;
      auto reader = JsonReader{};
      object_t results_ast;

      if (!reader.read(search_result.json_buffer, results_ast)) {
        TRACE_(11, "list_database_names() Failed to create an AST from search results JSON.");

        throw cet::exception("list_database_names") << "Failed to create an AST from search results JSON.";
      }

      auto const& results_list = boost::get<array_t>(results_ast.at(cfl::document::search));

      TRACE_(11, "list_database_names: found " << results_list.size() << " results.");

      std::ostringstream os;

      for (auto const& result_entry : results_list) {
        auto const& buff = boost::get<object_t>(result_entry).at(cfl::name);
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
  assert(filters.empty());
  assert(options.operation().compare(cflo::readdbinfo) == 0);

  TRACE_(12, "read_database_info: begin");
  TRACE_(11, "read_database_info args options=<" << options.to_string() << ">");

  validate_dbprovider_name(options.provider());

  auto dispatch_persistence_provider = [](std::string const& name) {
    auto providers = std::map<std::string, provider_readdbinfo_t>{
        {cflp::mongo, cf::mongo::readDatabaseInfo}, {cflp::filesystem, cf::filesystem::readDatabaseInfo}, {cflp::ucond, cf::ucond::readDatabaseInfo}};

    return providers.at(name);
  };

  auto search_result = dispatch_persistence_provider(options.provider())(options, options.search_filter_to_JsonData().json_buffer);

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
        throw cet::exception("read_database_info") << "Unsupported data format.";
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
  assert(collections.empty());
  assert(options.operation().compare(cflo::listcollections) == 0);

  TRACE_(13, "list_collection_names: begin");
  TRACE_(13, "list_collection_names args search_filter=<" << options.to_string() << ">");

  validate_dbprovider_name(options.provider());

  auto dispatch_persistence_provider = [](std::string const& name) {
    auto providers = std::map<std::string, provider_listcollections_t>{{cflp::mongo, cf::mongo::listCollectionNames},
                                                                       {cflp::filesystem, cf::filesystem::listCollectionNames},
                                                                       {cflp::ucond, cf::ucond::listCollectionNames}};

    return providers.at(name);
  };

  auto search_result = dispatch_persistence_provider(options.provider())(options, options.collectionName_to_JsonData().json_buffer);

  auto returnValue = std::string{};
  auto returnValueChanged = bool{false};

  switch (options.dataFormat()) {
    default:
    case data_format_t::db:
    case data_format_t::json:
    case data_format_t::unknown:
    case data_format_t::fhicl:
    case data_format_t::xml: {
      throw cet::exception("list_collection_names") << "Unsupported data format.";
      break;
    }

    case data_format_t::gui: {
      returnValue = search_result.json_buffer;
      returnValueChanged = true;
      break;
    }
    case data_format_t::console: {
      using namespace artdaq::database::json;
      auto reader = JsonReader{};
      object_t results_ast;

      if (!reader.read(search_result.json_buffer, results_ast)) {
        TRACE_(11, "list_collection_names() Failed to create an AST from search results JSON.");

        throw cet::exception("list_collection_names") << "Failed to create an AST from search results JSON.";
      }

      auto const& results_list = boost::get<array_t>(results_ast.at(cfl::document::search));

      TRACE_(11, "list_collection_names: found " << results_list.size() << " results.");

      std::ostringstream os;

      for (auto const& result_entry : results_list) {
        auto const& buff = boost::get<object_t>(result_entry).at(cfl::name);
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
