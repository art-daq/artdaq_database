#ifndef _ARTDAQ_DATABASE_MANAGED_CONFIGURATION_H_
#define _ARTDAQ_DATABASE_MANAGED_CONFIGURATION_H_

#include "artdaq-database/ConfigurationDB/common.h"
#include "artdaq-database/ConfigurationDB/configuration.h"

namespace artdaq{
namespace database{
namespace configuration{

template <typename TYPE, template <typename TYPE> class PROVIDER>
class ManagedConfiguration : public Configuration <TYPE, PROVIDER>
{
    using SelfType = ManagedConfiguration<TYPE,PROVIDER>;
    using UserType = TYPE;
    using PersistType = Configuration<TYPE,PROVIDER>;

public:
    template <typename ...ARGS>
    ManagedConfiguration(ARGS && ...args)
        : Configuration <TYPE,PROVIDER>(std::forward<ARGS>(args)...),
         _collection_name(Configuration<TYPE,PROVIDER>::versioned_typename()){}

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
    
    void useProvider(std::shared_ptr<PROVIDER<TYPE>> provider){_provider=provider;}
    
private:
    SelfType& self(){ 
      return *this;      
    }
    
    std::shared_ptr<PROVIDER<TYPE>>& provider(){
    assert(_provider);
    return _provider;
    }
    
private:    
    std::string _collection_name;
    std::shared_ptr<PROVIDER<TYPE>> _provider;
};

} //namespace configuration
} //namespace database
} //namespace artdaq
#endif /* _ARTDAQ_DATABASE_MANAGED_CONFIGURATION_H_ */
