#ifndef _ARTDAQ_DATABASE_MANAGED_CONFIGURATION_ALIAS_H_
#define _ARTDAQ_DATABASE_MANAGED_CONFIGURATION_ALIAS_H_

#include "artdaq-database/ConfigurationDB/common.h"
#include "artdaq-database/ConfigurationDB/configuration.h"

namespace artdaq{
namespace database{
namespace configuration{

template <typename TYPE, template <typename TYPE> class PROVIDER>
class ManagedConfigurationAlias : public Configuration <TYPE, PROVIDER>
{
    using SelfType = ManagedConfigurationAlias<TYPE, PROVIDER>;

public:
    template <typename ...ARGS>
    ManagedConfigurationAlias(ARGS && ...args)
        : Configuration <TYPE,PROVIDER>(std::forward<ARGS>(args)...),
         collection_name(TYPE::versioned_typename()){}
         
    template <typename FILTER>
    std::vector<SelfType> load(FILTER&);

    std::vector<SelfType> load();

    SelfType& store();

    template <typename ARG>
    SelfType& assignConfigAlias(ARG && arg) {
        Configuration<TYPE,PROVIDER>::self().assignConfigAlias(std::forward<ARG>(arg));
        return *this;
    }

    SelfType& markDeleted();

private:
    std::string collection_name;
};



} //namespace configuration
} //namespace database
} //namespace artdaq
#endif /* _ARTDAQ_DATABASE_MANAGED_CONFIGURATION_ALIAS_H_ */
