#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/ConfigurationDB/configurationdb.h"
#include "artdaq-database/ConfigurationDB/dispatch_common.h"
#include "artdaq-database/ConfigurationDB/options_operations.h"
#include "artdaq-database/DataFormats/Json/json_reader.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h"
#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb_index.h"
#include "artdaq-database/StorageProviders/MongoDB/provider_mongodb.h"

namespace t = artdaq::database::configuration::json;

void t::enable_trace() {
  artdaq::database::configuration::debug::ExportImport();
  artdaq::database::configuration::debug::ManageAliases();
  artdaq::database::configuration::debug::ManageConfigs();
  artdaq::database::configuration::debug::ManageDocuments();
  artdaq::database::configuration::debug::Metadata();

  artdaq::database::configuration::debug::detail::ExportImport();
  artdaq::database::configuration::debug::detail::ManageAliases();
  artdaq::database::configuration::debug::detail::ManageConfigs();
  artdaq::database::configuration::debug::detail::ManageDocuments();
  artdaq::database::configuration::debug::detail::Metadata();

  artdaq::database::configuration::debug::options::OperationBase();
  artdaq::database::configuration::debug::options::BulkOperations();
  artdaq::database::configuration::debug::options::ManageDocuments();
  artdaq::database::configuration::debug::options::ManageConfigs();
  artdaq::database::configuration::debug::options::ManageAliases();

  artdaq::database::configuration::debug::MongoDB();
  artdaq::database::configuration::debug::UconDB();
  artdaq::database::configuration::debug::FileSystemDB();

  artdaq::database::filesystem::debug::enable();
  artdaq::database::filesystem::index::debug::enable();
  artdaq::database::mongo::debug::enable();

  artdaq::database::docrecord::debug::JSONDocumentBuilder();
}