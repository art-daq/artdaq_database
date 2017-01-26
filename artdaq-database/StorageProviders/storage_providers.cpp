#include "artdaq-database/SharedCommon/helper_functions.h"
#include "artdaq-database/StorageProviders/storage_providers.h"

#include <sstream>

namespace db = artdaq::database;

std::string db::make_database_metadata(std::string const& name, std::string const& uri) {
  confirm(!name.empty());
  confirm(!uri.empty());

  // clang-format off
  std::ostringstream oss;
  oss << "document"_quoted
     << ":"
     << "{";
  oss << "name"_quoted
     << ":"
     << quoted_(name)
     << ",";
  oss << "uri"_quoted
     << ":"
     << quoted_(uri)
     << ",";
  oss << "create_time"_quoted
     << ":" << quoted_(timestamp());
  oss << "}";
  // clang-format on

  return oss.str();
}
