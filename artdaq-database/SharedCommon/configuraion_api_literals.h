#ifndef _ARTDAQ_DATABASE_CONFIGURATIONAPI_LITERALS_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONAPI_LITERALS_H_

namespace artdaq {
namespace database {
namespace result {
constexpr auto msg_EmptyFilter = "{\"message\":\"empty_filter\"}";
constexpr auto msg_EmptyDocument = "{\"message\":\"Json document is empty\"}";

constexpr auto msg_SystemCallFailed =
    "{\"message\":\"Unable to call system() from this API; check if either set-user-ID or set-group-ID check if either "
    "set-user-ID or set-group-ID \"}";
}
namespace configapi {
namespace literal {

constexpr auto operations = "operations";
constexpr auto database = "database";

namespace operation {
constexpr auto writeconfiguration = "globalconfstore";
constexpr auto readconfiguration = "globalconfload";

constexpr auto overwritedocument = "overwritedocument";
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

constexpr auto markdeleted = "markdeleted";
constexpr auto markreadonly = "markreadonly";

constexpr auto confcomposition = "buildfilter";
constexpr auto assignconfig = "addconfig";
constexpr auto removeconfig = "rmconfig";
constexpr auto newconfig = "newconfig";

constexpr auto addrun = "addrun";

constexpr auto addversionalias = "addveralias";
constexpr auto rmversionalias = "rmveralias";
constexpr auto findversionalias = "findveralias";

constexpr auto addconfigalias = "addconfigalias";
constexpr auto rmconfigalias = "rmconfigalias";
constexpr auto findconfigalias = "findconfigalias";

constexpr auto exportdatabase = "exportdatabase";
constexpr auto importdatabase = "importdatabase";
constexpr auto exportconfig = "exportconfig";
constexpr auto importconfig = "importconfig";
constexpr auto exportcollection = "exportcollection";
constexpr auto importcollection = "importcollection";
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
constexpr auto run = "run";

constexpr auto searchfilter = "filter";
constexpr auto searchquery = "searchquery";
constexpr auto bulkoperations = "bulkoperations";

constexpr auto collection = "collection";
constexpr auto entity = "entity";

constexpr auto configuration = "configuration";
constexpr auto configuration_alias = "configurationalias";
}

namespace filter {
constexpr auto entities = "entities.name";
constexpr auto configurations = "configurations.name";
constexpr auto version_aliases = "aliases.active.name";
constexpr auto configuration_aliases = "configaliases.active.name";
constexpr auto runs = "runs.name";

constexpr auto version = "version";
constexpr auto version_alias = "alias";
constexpr auto run = "run";
constexpr auto configuration_alias = "configuration_alias";
}

namespace format {
constexpr auto gui = "gui";
}

namespace msg {
constexpr auto empty_filter = "Search filter is empty";
constexpr auto empty_document = "Json document is empty";
constexpr auto cant_call_system =
    "Unable to call system() from this API; "
    "check if either set-user-ID or set-group-ID "
    "check if either set-user-ID or set-group-ID ";
}

constexpr auto name = "name";
constexpr auto notprovided = "notprovided";
constexpr auto apiname = "artdaq_database";
constexpr auto whitespace = " ";
constexpr auto nullstring = "";

constexpr auto empty_json = "{ }";
constexpr auto empty_search_result = "{\"search\":[ ]}";

constexpr auto empty_filesystem_index =
    "{\"ouid\":[], \"version\":{},\"configurations.name\":{}, "
    "\"entities.name\":{},\"aliases.active.name\":{}, "
    "\"runs.name\":{},\"configurations.assigned\":{} }";

constexpr auto bzip2base64 = "ascii.tar.bzip2.base64";
constexpr auto tmpdirprefix = "/tmp/adb";
constexpr auto dbexport_extension = ".tar-bzip2-base64";

constexpr auto database_format_version = 3;

constexpr auto database_format_locale = "en_US.UTF-8";

constexpr auto timestamp_format_old = "%a %b %d %H:%M:%S %Y";
constexpr auto timestamp_format= "%FT%T.000%z";
//constexpr auto timestamp_faketime_old = "Mon Feb  8 14:00:30 2016";
constexpr auto timestamp_faketime = "2017-07-18T12:48:10.123-0500";
//2017-07-18T12:48:10-0500
}
}  // namespace configapi
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_CONFIGURATIONAPI_LITERALS_H_ */
