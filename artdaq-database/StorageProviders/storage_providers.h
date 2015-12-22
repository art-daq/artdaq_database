#ifndef _ARTDAQ_DATABASE_PROVIDERS_H_
#define _ARTDAQ_DATABASE_PROVIDERS_H_

#include "artdaq-database/StorageProviders/common.h"

namespace artdaq {
namespace database {

template <typename TYPE, typename IMPL>
class StorageProvider final
{
public:
    StorageProvider (std::shared_ptr<IMPL> const& provider)
    :_provider(provider){}
    
    template <typename FILTER>
    std::vector<TYPE> load(FILTER const&);
    void store(TYPE const&);    
private:    
    std::shared_ptr<IMPL> _provider;
};
} //namespace database
} //namespace artdaq

#endif /* _ARTDAQ_DATABASE_PROVIDERS_H_ */
