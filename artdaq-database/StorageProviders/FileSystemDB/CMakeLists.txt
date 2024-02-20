cet_make_library(
	SOURCE
filesystem_functions.cpp
provider_connection.cpp
provider_filedb.cpp
provider_filedb_index.cpp
provider_filedb_readwrite.cpp
  LIBRARIES
  artdaq-database_SharedCommon
  artdaq-database_BasicTypes
  artdaq-database_JsonDocument
  artdaq-database_StorageProviders
  cetlib_except
)

install_headers()
install_source()
