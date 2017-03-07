#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h"
#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb_index.h"
#include "artdaq-database/StorageProviders/common.h"

#include "artdaq-database/SharedCommon/sharedcommon_common.h"

#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"

/*
#include <wordexp.h>
#include <boost/filesystem.hpp>
#include <boost/range/adaptors.hpp>
*/

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "PRVDR:FileDB_C"

namespace db = artdaq::database;
namespace dbfs = artdaq::database::filesystem;
namespace dbfsl = dbfs::literal;

using artdaq::database::filesystem::DBConfig;
using artdaq::database::filesystem::FileSystemDB;
using artdaq::database::filesystem::index::SearchIndex;

using artdaq::database::basictypes::JsonData;
using artdaq::database::docrecord::JSONDocumentBuilder;
using artdaq::database::docrecord::JSONDocument;

namespace jsonliteral = artdaq::database::dataformats::literal;

DBConfig::DBConfig()
    : uri{std::string{literal::FILEURI} + "${ARTDAQ_DATABASE_DATADIR}/filesystemdb" + "/" + literal::db_name} {
  auto tmpURI =
      getenv("ARTDAQ_DATABASE_URI") ? expand_environment_variables("${ARTDAQ_DATABASE_URI}") : std::string("");

  if (tmpURI.back() == '/') tmpURI.pop_back();  // remove trailing slash

  auto prefixURI = std::string{literal::FILEURI};

  if (tmpURI.length() > prefixURI.length() && std::equal(prefixURI.begin(), prefixURI.end(), tmpURI.begin()))
    uri = tmpURI;
}

DBConfig::DBConfig(std::string uri_) : uri{uri_} { confirm(!uri_.empty()); }

FileSystemDB::FileSystemDB(DBConfig const& config, PassKeyIdiom const&)
    : _config{config}, _client{_config.connectionURI()}, _connection{_config.connectionURI() + "/"} {}

std::string& FileSystemDB::connection() {
  auto collection = _connection + system_metadata;
  collection = expand_environment_variables(collection);

  if (collection.find(dbfsl::FILEURI) == 0) collection = collection.substr(strlen(dbfsl::FILEURI));

  auto path = boost::filesystem::path(collection.c_str());

  if (boost::filesystem::is_directory(path)) return _connection;

  auto oid = generate_oid();

  std::ostringstream oss;
  oss << "{";
  oss << make_database_metadata("artdaq", expand_environment_variables(_config.connectionURI())) << ",";
  oss << db::quoted_(jsonliteral::id) << ": { ";
  oss << db::quoted_(jsonliteral::oid) << ":" << db::quoted_(oid) << "}";
  oss << "}";

  auto filename = mkdir(collection) + "/" + oid + ".json";

  db::write_buffer_to_file(oss.str(), filename);

  TRACE_(5, "StorageProvider::FileSystemDB::connection created metadata record id=" << oid
                                                                                    << ", path=" << path.c_str());

  return _connection;
}