#include "artdaq-database/ConfigurationDB/managed_configuration.h"

using artdaq::database::configuration::ManagedConfiguration;
  
template <typename CONF, typename PROVIDER>
template <typename FILTER>
std::vector<typename ManagedConfiguration<CONF,PROVIDER>::SelfType>
ManagedConfiguration<CONF,PROVIDER>::load(FILTER& f)
{
   return provider()->load(f);
}

template <typename CONF, typename PROVIDER>
typename ManagedConfiguration<CONF,PROVIDER>::SelfType&
ManagedConfiguration<CONF,PROVIDER>::store()
{
  std::stringstream os;
  self().write(os);  
  provider()->store(StorableType(os.str()));
  return self();
}

template <typename CONF, typename PROVIDER>
typename ManagedConfiguration<CONF,PROVIDER>::SelfType&
ManagedConfiguration<CONF,PROVIDER>::assignAlias(std::string const&)
{
  return self();
}

template <typename CONF, typename PROVIDER>
typename ManagedConfiguration<CONF,PROVIDER>::SelfType&
ManagedConfiguration<CONF,PROVIDER>::addToGlobalConfiguration(std::string const&)
{
  return self();
}

template <typename CONF, typename PROVIDER>
typename ManagedConfiguration<CONF,PROVIDER>::SelfType&
ManagedConfiguration<CONF,PROVIDER>::markDeleted()
{
  return self();
}

template <typename CONF, typename PROVIDER>
std::vector<std::string>
ManagedConfiguration<CONF,PROVIDER>::listAliases()
{
  return {};
}

template <typename CONF, typename PROVIDER>        
std::vector<std::tuple<std::string, std::time_t>> 
ManagedConfiguration<CONF,PROVIDER>::listAliasHistory()
{
  return {};
}

template <typename CONF, typename PROVIDER>        
std::vector<std::string> 
ManagedConfiguration<CONF,PROVIDER>::listGlobalConfigurations()
{
  return {};
}	
	