#ifndef _ARTDAQ_DATABASE_MANAGED_CONFIGURATION_ALIAS_H_
#define _ARTDAQ_DATABASE_MANAGED_CONFIGURATION_ALIAS_H_

#include "artdaq-database/ConfigurationDB/common.h"
#include "artdaq-database/ConfigurationDB/configuration.h"

namespace artdaq{
namespace database{
namespace configuration{

template <typename CONF, template <typename CONF> class PROVIDER>
class ManagedConfigurationAlias : public Configuration <CONF, PROVIDER>
{
    using SelfType = ManagedConfigurationAlias<CONF, PROVIDER>;

public:
    template <typename ...ARGS>
    ManagedConfigurationAlias(ARGS && ...args)
        : Configuration <CONF,PROVIDER>(std::forward<ARGS>(args)...),
         collection_name(CONF::versioned_typename()){}
         
    template <typename FILTER>
    std::vector<SelfType> load(FILTER&);

    std::vector<SelfType> load();

    SelfType& store();

    template <typename ARG>
    SelfType& assignConfigAlias(ARG && arg) {
        Configuration<CONF,PROVIDER>::self().assignConfigAlias(std::forward<ARG>(arg));
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
