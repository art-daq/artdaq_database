#ifndef _ARTDAQ_DATABASE_CONFIGURATIONDB_SHARED_LITERALS_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONDB_SHARED_LITERALS_H_

namespace artdaq {
namespace database {
namespace configuration {
namespace literal {

namespace operation {
constexpr auto store = "store";
constexpr auto load = "load";

constexpr auto findconfigs = "findconfigs";
constexpr auto findversions = "findversions";
constexpr auto findentities = "findentities";

constexpr auto buildfilter = "buildfilter";
constexpr auto addconfig = "addconfig";
constexpr auto newconfig = "newconfig";

constexpr auto addalias = "addalias";
constexpr auto rmalias = "rmalias";
constexpr auto findalias = "findalias";
}

namespace provider {
constexpr auto mongo = "mongo";
constexpr auto filesystem = "filesystem";
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

constexpr auto version = "version";
constexpr auto version_alias = "alias";
constexpr auto searchfilter = "filter";
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
}

namespace search {
constexpr auto default_filter = "{\"configurable_entity\":\"any\"}";
}

constexpr auto notprovided = "notprovided";
constexpr auto unknown = "unknown";
constexpr auto collection_default = "test_Data_Type001";
constexpr auto database_provider_mongo = "mongo";
constexpr auto database_provider_filesystem = "filesystem";
constexpr auto colon = ":";
constexpr auto empty_json = "{ }";
constexpr auto empty_search_result = "{\"search\":[ ]}";
}
}  // namespace configuration
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_CONFIGURATIONDB_SHARED_LITERALS_H_ */
