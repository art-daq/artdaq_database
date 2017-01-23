#ifndef _ARTDAQ_DATABASE_CONFIGURATIONAPI_LITERALS_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONAPI_LITERALS_H_

namespace artdaq {
namespace database {
namespace result {
constexpr auto msg_EmptyFilter = "{\"message\":\"empty_filter\"}";
constexpr auto msg_EmptyDocument = "{\"message\":\"Json document is empty\"}";
constexpr auto msg_SystemCallFailed = "{\"message\":\"Unable to call system() from this API; check if either set-user-ID or set-group-ID check if either set-user-ID or set-group-ID \"}";
}
namespace configapi {
namespace literal {

constexpr auto operations = "operations";
  
namespace operation {
constexpr auto writeconfiguration = "globalconfstore";
constexpr auto readconfiguration = "globalconfload";

constexpr auto writedocument = "store";
constexpr auto readdocument = "load";

constexpr auto listcollections = "listcollections";
constexpr auto listdatabases = "listdatabases";
constexpr auto readdbinfo = "readdbinfo";

constexpr auto findconfigs = "findconfigs";
constexpr auto findversions = "findversions";

constexpr auto findentities = "findentities";
constexpr auto addentity = "addentity";
constexpr auto rmentity = "rmentity";

constexpr auto confcomposition = "buildfilter";
constexpr auto assignconfig = "addconfig";
constexpr auto removeconfig = "rmconfig";
constexpr auto newconfig = "newconfig";

constexpr auto addalias = "addalias";
constexpr auto rmalias = "rmalias";
constexpr auto findalias = "findalias";
}

namespace provider {
constexpr auto mongo = "mongo";
constexpr auto filesystem = "filesystem";
constexpr auto ucon = "ucon";
}

namespace option {
constexpr auto operation = "operation";
constexpr auto format = "dataformat";
constexpr auto provider = "dbprovider";

constexpr auto result = "result";
constexpr auto source = "source";

constexpr auto version = "version";
constexpr auto version_alias = "alias";
constexpr auto searchfilter = "filter";
constexpr auto searchquery = "searchquery";
constexpr auto bulkoperations = "bulkoperations";

constexpr auto collection = "collection";
constexpr auto entity = "entity";
constexpr auto configuration = "configuration";
constexpr auto configuration_alias = "configurationalias";
}

namespace filter {
constexpr auto entity = "entities.name";
constexpr auto version = "version";
constexpr auto version_alias = "alias";
constexpr auto configuration = "configurations.name";
constexpr auto configuration_alias = "configuration_alias";
}

namespace gui {
constexpr auto entity = "entities";
constexpr auto operations = "operations";
}

namespace msg {
constexpr auto empty_filter = "Search filter is empty";
constexpr auto empty_document = "Json document is empty";
constexpr auto cant_call_system =
    "Unable to call system() from this API; "
    "check if either set-user-ID or set-group-ID "
    "check if either set-user-ID or set-group-ID ";
}

namespace search {
constexpr auto default_filter = "{\"entities\":\"any\"}";
}



constexpr auto name = "name";

constexpr auto filterx = "filter";
constexpr auto notprovided = "notprovided";
constexpr auto unknown = "unknown";
constexpr auto collection_default = "test_Data_Type001";

constexpr auto colon = ":";
constexpr auto empty_json = "{ }";
constexpr auto empty_search_result = "{\"search\":[ ]}";
}
}  // namespace configapi
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_CONFIGURATIONAPI_LITERALS_H_ */
