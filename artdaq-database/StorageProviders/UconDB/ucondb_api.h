#ifndef _ARTDAQ_DATABASE_PROVIDER_UCONDAPI_H_
#define _ARTDAQ_DATABASE_PROVIDER_UCONDAPI_H_

#include "artdaq-database/StorageProviders/UconDB/provider_ucondb.h"
#include "artdaq-database/StorageProviders/storage_providers.h"

namespace artdaq {
namespace database {
namespace ucon {
namespace debug {
void enableUconDBAPI();
}

using artdaq::database::ucon::UconDBSPtr_t;

std::list<std::string> folders(UconDBSPtr_t provider);
std::list<std::string> tags(UconDBSPtr_t provider, std::string const& folder);
std::list<std::string> objects(UconDBSPtr_t provider, std::string const& folder);
std::string get_object(UconDBSPtr_t provider, std::string const& folder, std::string const& object);
}  // namespace ucon
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_PROVIDER_UCONDAPI_H_ */
