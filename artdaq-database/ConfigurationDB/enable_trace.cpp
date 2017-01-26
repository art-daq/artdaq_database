#include "artdaq-database/ConfigurationDB/configurationdb.h"
#include "artdaq-database/ConfigurationDB/options_operations.h"
#include "artdaq-database/ConfigurationDB/dispatch_common.h"
#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h"
#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb_index.h"
#include "artdaq-database/StorageProviders/MongoDB/provider_mongodb.h"
#include "artdaq-database/DataFormats/Json/json_reader.h"

namespace t = artdaq::database::configuration::json;

void t::enable_trace(){
  artdaq::database::filesystem::debug::enable();
  artdaq::database::mongo::debug::enable();
  artdaq::database::docrecord::debug::enableJSONDocumentBuilder();

  artdaq::database::configuration::debug::options::enableOperationBase();

  artdaq::database::configuration::debug::enableFindConfigsOperation();
  artdaq::database::configuration::debug::enableCreateConfigsOperation();
  artdaq::database::configuration::debug::enableManageDocumentOperation();

  artdaq::database::configuration::debug::detail::enableCreateConfigsOperation();
  artdaq::database::configuration::debug::detail::enableFindConfigsOperation();
  artdaq::database::configuration::debug::detail::enableManageDocumentOperation();

  artdaq::database::configuration::debug::options::enableOperationManageConfigs();
  
  artdaq::database::configuration::debug::enableDBOperationMongo();
  artdaq::database::configuration::debug::enableDBOperationFileSystem();
  artdaq::database::filesystem::index::debug::enable();

  artdaq::database::configuration::debug::enableDBOperationUcond();
}