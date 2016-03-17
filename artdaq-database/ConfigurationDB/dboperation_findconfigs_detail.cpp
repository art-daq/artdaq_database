#include "artdaq-database/ConfigurationDB/common.h"

#include "artdaq-database/ConfigurationDB/dboperation_filedb.h"
#include "artdaq-database/ConfigurationDB/dboperation_findconfigs.h"
#include "artdaq-database/ConfigurationDB/dboperation_mongodb.h"

#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/FhiclJson/shared_literals.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/JsonDocument/JSONDocument_template.h"

#include <boost/exception/diagnostic_information.hpp>

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "CONF:OpSrchD_C"

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
void find_global_configurations(FindConfigsOperation const& options, std::string& configs) {
  assert(configs.empty());
  assert(options.operation().compare(cfo::literal::operation_findconfigs) == 0);

  TRACE_(11, "find_global_configurations: begin");
  TRACE_(11, "find_global_configurations args options=<" << options.to_string() << ">");

  configs="{\"aaa\":\"aaa\"}";
}
void build_global_configuration_search_filter(FindConfigsOperation const& options, std::string& filter) {
  assert(filter.empty());
  assert(options.operation().compare(cfo::literal::operation_buildfilter) == 0);

  TRACE_(12, "build_global_configuration_search_filter: begin");
  TRACE_(11, "build_global_configuration_search_filter args options=<" << options.to_string() << ">");
  
  filter="{\"aaa\":\"aaa\"}";

}
}

void cf::trace_enable_FindConfigsOperationDetail() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", 1LL);
  TRACE_CNTL("modeS", 1LL);

  TRACE_(0, "artdaq::database::configuration::FindConfigsOperationDetail"
                << "trace_enable");
}
