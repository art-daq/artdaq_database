cet_make_library(
  SOURCE
  helper_functions.cpp
  mongo_functions.cpp
  mongo_json.cpp
  provider_connection.cpp
  provider_mongodb.cpp
  provider_mongodb_readwrite.cpp
  LIBRARIES PUBLIC
  artdaq-database::SharedCommon
  artdaq-database::BasicTypes
  artdaq-database::JsonDocument
  artdaq-database::StorageProviders
  mongo::mongocxx_shared
  mongo::bsoncxx_shared
)

install_headers()
install_source()
