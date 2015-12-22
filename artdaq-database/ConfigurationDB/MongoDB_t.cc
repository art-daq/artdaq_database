#include "artdaq-database/ConfigurationDB/common.h"
#include "artdaq-database/BasicTypes/basictypes.h"

#include "artdaq-database/ConfigurationDB/managed_configuration_impl.h"
#include "artdaq-database/ConfigurationDB/provider_mongodb.h"
#include "artdaq-database/ConfigurationDB/demo_configuration_types_v1.h"


int main(int argc [[gnu::unused]], char * argv[] [[gnu::unused]]) try
{
  using  artdaq::database::configuration::Configuration;
  using  artdaq::database::configuration::ManagedConfiguration;
  using  artdaq::database::configuration::FhiclData;
  using  artdaq::database::configuration::JsonData;
  using  artdaq::database::MongoDB;
  
  using  FhiclConfiguration = Configuration<FHICLData,JSON>;
  
  
  FHICLData fhicl = {"aa:12"};
  
  
 
  using  FHICL = ManagedConfiguration<FHICLConfig,MongoDB>;
  
  
 /* 
  
  FHICLData fhicl = {"aa:12"};
  auto config = FHICLConfig(fhicl);
  
  auto configs = config.load();
  */
  
  return 0;
}

catch (std::string & x)
{
  std::cerr << "Exception (type string) caught in main: " << x << "\n";
  return 1;
}

catch (char const * m)
{
  std::cerr << "Exception (type char const*) caught in main: " << std::endl;
  if (m)
    { std::cerr << m; }
  else
    { std::cerr << "[the value was a null pointer, so no message is available]"; }
  std::cerr << '\n';
}