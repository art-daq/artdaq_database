#ifndef _ARTDAQ_DATABASE_CONFIGURATIONAPI_LITERALS_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONAPI_LITERALS_H_

#include "artdaq-database/SharedCommon/common.h"

namespace artdaq {
namespace database {
namespace configapi {
namespace literal {

namespace operation {
constexpr auto globalconfstore = "globalconfstore";
constexpr auto globalconfload = "globalconfload";

constexpr auto store = "store";
constexpr auto load = "load";

constexpr auto listcollections = "listcollections";
constexpr auto listdatabases = "listdatabases";
constexpr auto readdbinfo = "readdbinfo";

constexpr auto findconfigs = "findconfigs";
constexpr auto findversions = "findversions";
constexpr auto findentities = "findentities";

constexpr auto buildfilter = "buildfilter";
constexpr auto addconfig = "addconfig";
constexpr auto newconfig = "newconfig";

constexpr auto addalias = "addalias";
constexpr auto rmalias = "rmalias";
constexpr auto findalias = "findalias";
constexpr auto operations = "operations";
}

namespace provider {
constexpr auto mongo = "mongo";
constexpr auto filesystem = "filesystem";
constexpr auto ucond = "ucond";
}

namespace result {
constexpr auto SUCCESS = "Success";
constexpr auto FAILURE = "Failure";
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
constexpr auto entity = "configurable_entity.name";
constexpr auto version = "version";
constexpr auto version_alias = "alias";
constexpr auto configuration = "configurations.name";
constexpr auto configuration_alias = "configuration_alias";
}

namespace gui {
constexpr auto configurable_entity = "configurable_entity";
constexpr auto operations = "operations";
}

namespace msg {
constexpr auto empty_filter = "Search filter is empty";
constexpr auto empty_document = "Json document is empty";
constexpr auto cant_call_system =
    "Unable to call system() from this API; "
    "check if either set-user-ID or set-group-ID "
    "are used in the program execution context.";
}

namespace search {
constexpr auto default_filter = "{\"configurable_entity\":\"any\"}";
}

namespace document {
constexpr auto document = "document";
constexpr auto data = "data";
constexpr auto metadata = "metadata";
constexpr auto search = "search";
constexpr auto query = "query";
}

namespace origin {
constexpr auto origin = "origin";
constexpr auto source = "source";
constexpr auto timestamp = "timestamp";
constexpr auto format = "format";
constexpr auto rawdata = "rawdata";
constexpr auto filename = "filename";
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
