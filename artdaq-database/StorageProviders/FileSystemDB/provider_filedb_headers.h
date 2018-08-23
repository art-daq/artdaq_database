#ifndef _ARTDAQ_DATABASE_PROVIDER_FILESYSTEM_HEADERS_H_
#define _ARTDAQ_DATABASE_PROVIDER_FILESYSTEM_HEADERS_H_

#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h"
#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb_index.h"
#include "artdaq-database/StorageProviders/common.h"

#include "artdaq-database/SharedCommon/sharedcommon_common.h"
#include "artdaq-database/DataFormats/shared_literals.h"

#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"

namespace db = artdaq::database;
namespace dbfs = artdaq::database::filesystem;
namespace dbfsl = dbfs::literal;

using artdaq::database::filesystem::DBConfig;
using artdaq::database::filesystem::FileSystemDB;
using artdaq::database::filesystem::index::SearchIndex;

using artdaq::database::docrecord::JSONDocument;
using artdaq::database::docrecord::JSONDocumentBuilder;

namespace jsonliteral = artdaq::database::dataformats::literal;

#endif /* _ARTDAQ_DATABASE_PROVIDER_FILESYSTEM_HEADERS_H_ */
