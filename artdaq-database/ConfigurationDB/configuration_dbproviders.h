#ifndef _ARTDAQ_DATABASE_CONFIGURATIONDB_DBPROVIDERS_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONDB_DBPROVIDERS_H_

#include "artdaq-database/ConfigurationDB/dispatch_filedb.h"
#include "artdaq-database/ConfigurationDB/dispatch_mongodb.h"
#include "artdaq-database/ConfigurationDB/dispatch_ucondb.h"

namespace artdaq {
namespace database {
namespace configuration {
void validate_dbprovider_name(std::string const&);
}  // namespace configuration
}  // namespace database
}  // namespace artdaq
#endif /* _ARTDAQ_DATABASE_CONFIGURATIONDB_DBPROVIDERS_H_ */
