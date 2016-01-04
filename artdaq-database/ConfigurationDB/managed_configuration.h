#ifndef _ARTDAQ_DATABASE_MANAGED_CONFIGURATION_H_
#define _ARTDAQ_DATABASE_MANAGED_CONFIGURATION_H_

#include "artdaq-database/ConfigurationDB/common.h"
#include "artdaq-database/ConfigurationDB/configuration.h"

namespace artdaq{
namespace database{
namespace configuration{

template <typename CONF>
using ConfigurationImpl = Configuration <CONF, JSON>;
  
template <typename CONF, typename PROVIDER>
class ManagedConfiguration : public ConfigurationImpl<CONF>
{
    using UserType = CONF;
    using StorableType = typename PROVIDER::StorableType;
    using SelfType = ManagedConfiguration<CONF,PROVIDER>;
    using PersistType = ConfigurationImpl<CONF>;
    
public:
    ManagedConfiguration(CONF const& conf, std::shared_ptr<PROVIDER> const& provider)
        : ConfigurationImpl<CONF>(conf),
         _collection_name(ConfigurationImpl<CONF>::versioned_typename()),
         _provider(provider){}

    template <typename FILTER>
    std::vector<SelfType> load(FILTER&);

    template <typename FILTER>
    SelfType unique(FILTER& f) {
        auto collection = load(f);
        assert(!collection.empty());
        return *collection.begin();
    }
    
    SelfType& store();
    SelfType& assignAlias(std::string const&);
    SelfType& addToGlobalConfiguration(std::string const&);
    SelfType& markDeleted();

    std::vector<std::string> listAliases();
    std::vector<std::tuple<std::string, std::time_t>> listAliasHistory();
    std::vector<std::string> listGlobalConfigurations();
    
private:
    SelfType& self(){ 
      return *this;      
    }
    
    std::shared_ptr<PROVIDER>& provider(){
    assert(_provider);
    return _provider;
    }
    
private:    
    std::string _collection_name;
    std::shared_ptr<PROVIDER> _provider;
};

} //namespace configuration
} //namespace database
} //namespace artdaq
#endif /* _ARTDAQ_DATABASE_MANAGED_CONFIGURATION_H_ */
