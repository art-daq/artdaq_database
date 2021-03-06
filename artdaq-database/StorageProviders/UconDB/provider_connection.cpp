#include <utility>

#include "artdaq-database/BasicTypes/basictypes.h"
#include "artdaq-database/JsonDocument/JSONDocumentBuilder.h"
#include "artdaq-database/SharedCommon/common.h"
#include "artdaq-database/StorageProviders/UconDB/provider_ucondb.h"

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "provider_connection.cpp"

namespace db = artdaq::database;
namespace dbucl = db::ucon::literal;

using artdaq::database::ucon::DBConfig;
using artdaq::database::ucon::UconDB;

DBConfig::DBConfig() : uri{std::string{dbucl::UCONURI} + dbucl::hostname + ":" + std::to_string(dbucl::port) + "/" + dbucl::db_name} {
  auto tmpURI = getenv("ARTDAQ_DATABASE_URI") != nullptr ? expand_environment_variables("${ARTDAQ_DATABASE_URI}") : std::string("");

  auto prefixURI = std::string{dbucl::UCONURI};

  if (tmpURI.length() > prefixURI.length() && std::equal(prefixURI.begin(), prefixURI.end(), tmpURI.begin())) {
    uri = tmpURI;
  }
}

DBConfig::DBConfig(const std::string& uri_) : uri{uri_} { confirm(!uri_.empty()); }

std::string& UconDB::connection() { return _connection; }

long UconDB::timeout() const { return 5; }
std::string const& UconDB::authentication() const { return _userpass; }

UconDB::UconDB(DBConfig config, PassKeyIdiom const& /*unused*/)
    : _config{std::move(config)}, _client{_config.connectionURI()}, _connection{_config.connectionURI()}, _userpass{"user:pass"} {
  _userpass = getenv("ARTDAQ_DATABASE_AUTH") != nullptr ? getenv("ARTDAQ_DATABASE_AUTH") : std::string("user:pass");
}
