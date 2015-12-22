#include "artdaq-database/ConfigurationDB/managed_configuration.h"

using artdaq::database::configuration::ManagedConfiguration;
  
template <typename TYPE, template <typename TYPE> class PROVIDER>
template <typename FILTER>
std::vector<typename ManagedConfiguration<TYPE,PROVIDER>::SelfType>
ManagedConfiguration<TYPE,PROVIDER>::load(FILTER& f)
{
   return provider()->load(f);
}

template <typename TYPE, template <typename TYPE> class PROVIDER>
typename ManagedConfiguration<TYPE,PROVIDER>::SelfType&
ManagedConfiguration<TYPE,PROVIDER>::store()
{
  provider()->store(self());
  return self();
}

template <typename TYPE, template <typename TYPE> class PROVIDER>
typename ManagedConfiguration<TYPE,PROVIDER>::SelfType&
ManagedConfiguration<TYPE,PROVIDER>::assignAlias(std::string const&)
{
  return self();
}

template <typename TYPE, template <typename TYPE> class PROVIDER>
typename ManagedConfiguration<TYPE,PROVIDER>::SelfType&
ManagedConfiguration<TYPE,PROVIDER>::addToGlobalConfiguration(std::string const&)
{
  return self();
}

template <typename TYPE, template <typename TYPE> class PROVIDER>
typename ManagedConfiguration<TYPE,PROVIDER>::SelfType&
ManagedConfiguration<TYPE,PROVIDER>::markDeleted()
{
  return self();
}

template <typename TYPE, template <typename TYPE> class PROVIDER>
std::vector<std::string>
ManagedConfiguration<TYPE,PROVIDER>::listAliases()
{
  return {};
}

template <typename TYPE, template <typename TYPE> class PROVIDER>        
std::vector<std::tuple<std::string, std::time_t>> 
ManagedConfiguration<TYPE,PROVIDER>::listAliasHistory()
{
  return {};
}

template <typename TYPE, template <typename TYPE> class PROVIDER>        
std::vector<std::string> 
ManagedConfiguration<TYPE,PROVIDER>::listGlobalConfigurations()
{
  return {};
}	
	