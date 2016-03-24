#include "artdaq-database/ConfigurationDB/common.h"

#include "artdaq-database/ConfigurationDB/dboperation_filedb.h"
#include "artdaq-database/ConfigurationDB/dboperation_findconfigs.h"
#include "artdaq-database/ConfigurationDB/dboperation_createconfigs.h"

#include "artdaq-database/ConfigurationDB/dboperation_mongodb.h"

#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/FhiclJson/shared_literals.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/JsonDocument/JSONDocument_template.h"

#include <boost/exception/diagnostic_information.hpp>

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "CONF:OpCrGCoD_C"

namespace db = artdaq::database;
namespace dbjsu = db::jsonutils;
namespace dbjsul = db::jsonutils::literal;
namespace cf = db::configuration;
namespace cfgui = cf::guiexports;
namespace cfo = cf::options;
namespace cfol = cfo::literal;

using cfo::FindConfigsOperation;
using cfo::data_format_t;

using Options = cfo::FindConfigsOperation;

using artdaq::database::basictypes::JsonData;
using artdaq::database::basictypes::FhiclData;
using artdaq::database::jsonutils::JSONDocument;
using artdaq::database::jsonutils::JSONDocumentBuilder;

namespace artdaq {
namespace database {
bool json_db_to_gui(std::string const&, std::string&);
bool json_gui_to_db(std::string const&, std::string&);
}
}

namespace detail {

typedef JsonData (*provider_findgversions_t)(Options const& /*options*/, JsonData const& /*search_filter*/);
typedef JsonData (*provider_newglobalconfig_t)(Options const& /*options*/, JsonData const& /*search_filter*/);
typedef JsonData (*provider_addtoglobalconfig_t)(Options const& /*options*/, JsonData const& /*search_filter*/);

void add_configuration_to_global_configuration(FindConfigsOperation const& options, std::string& configs [[gnu::unused]]){
  assert(configs.empty());
  assert(options.operation().compare(cfo::literal::operation_findconfigs) == 0);

  TRACE_(11, "add_configuration_to_global_configuration: begin");
  TRACE_(11, "add_configuration_to_global_configuration args options=<" << options.to_string() << ">");

  if (cf::not_equal(options.provider(), cfol::database_provider_filesystem) &&
      cf::not_equal(options.provider(), cfol::database_provider_mongo)) {
    TRACE_(11, "Error in add_configuration_to_global_configuration:"
                   << " Invalid database provider; database provider=" << options.provider() << ".");

    throw cet::exception("add_configuration_to_global_configuration")
        << "Invalid database provider; database provider=" << options.provider() << ".";
  }

  TRACE_(11, "add_configuration_to_global_configuration: end");
}


void create_new_global_configuration(FindConfigsOperation const& options, std::string& configs[[gnu::unused]]) {
  assert(configs.empty());
  assert(options.operation().compare(cfo::literal::operation_findconfigs) == 0);

  TRACE_(11, "create_new_global_configuration: begin");
  TRACE_(11, "create_new_global_configuration args options=<" << options.to_string() << ">");

  if (cf::not_equal(options.provider(), cfol::database_provider_filesystem) &&
      cf::not_equal(options.provider(), cfol::database_provider_mongo)) {
    TRACE_(11, "Error in create_new_global_configuration:"
                   << " Invalid database provider; database provider=" << options.provider() << ".");

    throw cet::exception("create_new_global_configuration")
        << "Invalid database provider; database provider=" << options.provider() << ".";
  }

  TRACE_(11, "create_new_global_configuration: end");
}

void find_configuration_versions(FindConfigsOperation const& options, std::string& configs[[gnu::unused]]) {
  assert(configs.empty());
  assert(options.operation().compare(cfo::literal::operation_findconfigs) == 0);

  TRACE_(11, "find_configuration_versions: begin");
  TRACE_(11, "find_configuration_versions args options=<" << options.to_string() << ">");

  if (cf::not_equal(options.provider(), cfol::database_provider_filesystem) &&
      cf::not_equal(options.provider(), cfol::database_provider_mongo)) {
    TRACE_(11, "Error in find_configuration_versions:"
                   << " Invalid database provider; database provider=" << options.provider() << ".");

    throw cet::exception("find_configuration_versions")
        << "Invalid database provider; database provider=" << options.provider() << ".";
  }

  TRACE_(11, "find_configuration_versions: end");
}
}

void cf::trace_enable_CreateConfigsOperationDetail() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", 1LL);
  TRACE_CNTL("modeS", 1LL);

  TRACE_(0, "artdaq::database::configuration::CreateConfigsOperationDetail"
                << "trace_enable");
}
