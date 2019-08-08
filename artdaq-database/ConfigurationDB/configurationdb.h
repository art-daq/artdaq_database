#ifndef _ARTDAQ_DATABASE_CONFIGURATIONDB_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONDB_H_

#include "artdaq-database/ConfigurationDB/dboperation_exportimport.h"
#include "artdaq-database/ConfigurationDB/dboperation_managealiases.h"
#include "artdaq-database/ConfigurationDB/dboperation_manageconfigs.h"
#include "artdaq-database/ConfigurationDB/dboperation_managedocument.h"
#include "artdaq-database/ConfigurationDB/dboperation_metadata.h"
#include "artdaq-database/ConfigurationDB/dboperation_searchcollection.h"

namespace artdaq {
namespace database {
namespace configuration {
namespace json {
void enable_trace();
}
}  // namespace configuration
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_CONFIGURATIONDB_H_ */
