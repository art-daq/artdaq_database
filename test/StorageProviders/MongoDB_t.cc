#include "artdaq-database/StorageProviders/common.h"
#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/BuildInfo/printStackTrace.h"

#include "artdaq-database/StorageProviders/storage_providers.h"
#include "artdaq-database/StorageProviders/MongoDB/provider_mongodb.h"
#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h"


int main(int argc [[gnu::unused]], char * argv[] [[gnu::unused]])
{
   registerAbortHandler();
   registerUncaughtExceptionHandler();

  using  artdaq::database::basictypes::FhiclData;
  using  artdaq::database::basictypes::JsonData;
  
  namespace DBI= artdaq::database::mongo;
  
  auto config =DBI::DBConfig{};  
  auto database = DBI::DB::create(config);  
  auto provider = DBI::DBProvider<JsonData>(database);
  
  auto json1 = JsonData{"{\"document\":{\"aa\":12}, \"collection\":\"test_V100\"}"};

  provider.store(json1);

  auto json2 = JsonData{"{\"document\":{\"aa\":14}, \"collection\":\"test_V100\", \"test_V100_id\":0}"};

  provider.store(json2);
  
  auto json3 = JsonData{"{\"filter\":{\"test_V100_id\":0, \"document.aa\":14}, \"collection\":\"test_V100\" }"};
  
  auto collection = provider.load(json3);
  
  for (auto&& element : collection) {
	    std::cout << element.json_buffer  << std::endl;
  }
  
  return 0;
}
