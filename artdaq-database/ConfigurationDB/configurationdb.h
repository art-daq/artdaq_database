#ifndef _ARTDAQ_DATABASE_CONFIGURATIONDB_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONDB_H_

#include "artdaq-database/ConfigurationDB/dboperation_globalconf_loadstore.h"
#include "artdaq-database/ConfigurationDB/dboperation_loadstore.h"
#include "artdaq-database/ConfigurationDB/dboperation_managealiases.h"
#include "artdaq-database/ConfigurationDB/dboperation_manageconfigs.h"
#include "artdaq-database/ConfigurationDB/dboperation_metadata.h"

namespace artdaq {
namespace database {
namespace jsonutils {
bool isValidJson(std::string const&);
}
}
}
#endif /* _ARTDAQ_DATABASE_CONFIGURATIONDB_H_ */
