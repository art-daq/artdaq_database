cet_make_library(LIBRARY_NAME artdaq-database_StorageProviders_MongoDB
  SOURCE
  helper_functions.cpp
  mongo_functions.cpp
  mongo_json.cpp
  provider_connection.cpp
  provider_mongodb.cpp
  provider_mongodb_readwrite.cpp
  LIBRARIES PUBLIC
  artdaq-database_SharedCommon
  artdaq-database_BasicTypes
  artdaq-database_JsonDocument
  artdaq-database_StorageProviders
  mongo::mongocxx_shared
  mongo::bsoncxx_shared
  cetlib_except
)

install_headers()
install_source()
