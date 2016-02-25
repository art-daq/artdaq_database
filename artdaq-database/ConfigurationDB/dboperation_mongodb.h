#ifndef _ARTDAQ_DATABASE_CONFIGURATIONDB_LOADSTORE_MONGO_H_
#define _ARTDAQ_DATABASE_CONFIGURATIONDB_LOADSTORE_MONGO_H_

#include "artdaq-database/ConfigurationDB/common.h"

namespace artdaq{
namespace database{
namespace basictypes{
class  JsonData;
}//namespace basictypes
namespace configuration{
namespace options{
class LoadStoreOperation;
}//namespace options

namespace mongo{
using Options= artdaq::database::configuration::options::LoadStoreOperation;
using artdaq::database::basictypes::JsonData;

void store ( Options const& /*options*/, JsonData const& /*insert_payload*/ );
JsonData load ( Options const& /*options*/, JsonData const& /*search_payload*/);

} //namespace mongo
} //namespace configuration
} //namespace database
} //namespace artdaq
#endif /* _ARTDAQ_DATABASE_CONFIGURATIONDB_LOADSTORE_MONGO_H_ */
