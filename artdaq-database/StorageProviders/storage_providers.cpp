#include "artdaq-database/SharedCommon/helper_functions.h"
#include "artdaq-database/StorageProviders/storage_providers.h"
#include "artdaq-database/SharedCommon/configuraion_api_literals.h"

#include <sstream>

namespace db = artdaq::database;
namespace apiliteral = db::configapi::literal;

std::string db::make_database_metadata(std::string const& name, std::string const& uri) {
  confirm(!name.empty());
  confirm(!uri.empty());

  auto user = expand_environment_variables("$USER");
  user.pop_back();
  
  // clang-format off
  std::ostringstream oss;
  oss << "document"_quoted << ":" << "{";
  oss << "name"_quoted     << ":" << quoted_(name) << ",";
  oss << "uri"_quoted      << ":" << quoted_(uri)  << ",";
  oss << "create_time"_quoted << ":" << quoted_(timestamp())  << ",";
  oss << "create_user"_quoted << ":" << quoted_(user)  << ",";
  oss << "uname"_quoted << ":" << unamejson() << ",";
  oss << "database_format"_quoted << ":" << apiliteral::database_format_version;
  oss << "}";  
  // clang-format on

  return oss.str();
}
