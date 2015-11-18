#ifndef artdaq_database_BuildInfo_GetPackageBuildInfo_hh
#define artdaq_database_BuildInfo_GetPackageBuildInfo_hh

#include <string>
#include "artdaq-core/Data/PackageBuildInfo.hh"

namespace artdaqdatabase{

struct GetPackageBuildInfo {

    static artdaq::PackageBuildInfo getPackageBuildInfo();
};

}

#endif /* artdaq_database_BuildInfo_GetPackageBuildInfo_hh */
