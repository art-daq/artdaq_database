cet_make_library(
	SOURCE
filesystem_functions.cpp
provider_connection.cpp
provider_filedb.cpp
provider_filedb_index.cpp
provider_filedb_readwrite.cpp
  LIBRARIES
  artdaq-database::SharedCommon
  artdaq-database::BasicTypes
  artdaq-database::JsonDocument
  artdaq-database::StorageProviders
)

install_headers()
install_source()
