#ifndef artdaq_database_BuildInfo_GetPackageBuildInfo_hh
#define artdaq_database_BuildInfo_GetPackageBuildInfo_hh

#include <string>
#include "artdaq-core/Data/PackageBuildInfo.hh"

/**
* \brief Namespace used to differentiate the artdaq_database version of GetPackageBuildInfo
* from other versions present in the system.
*/
namespace artdaqdatabase {

	/**
	* \brief Wrapper around the artdaqdatabase::GetPackageBuildInfo::getPackageBuildInfo function
	*/
struct GetPackageBuildInfo {
	/**
	* \brief Gets the version number and build timestmap for artdaq_database
	* \return An artdaq::PackageBuildInfo object containing the version number and build timestamp for artdaq_database
	*/
  static artdaq::PackageBuildInfo getPackageBuildInfo();
};
}

#endif /* artdaq_database_BuildInfo_GetPackageBuildInfo_hh */
