FIND_PACKAGE(CURL REQUIRED EXPORT)

IF(NOT CURL_FOUND)
  MESSAGE(FATAL_ERROR "Could not find the CURL library.")
ENDIF()

cet_make_library(
	SOURCE
provider_connection.cpp
provider_ucondb.cpp
provider_ucondb_readwrite.cpp
ucondb_api.cpp
  LIBRARIES
  artdaq-database_SharedCommon
  artdaq-database_BasicTypes
  artdaq-database_JsonDocument
  artdaq-database_StorageProviders
  CURL::libcurl
  cetlib_except
)

install_headers()
install_source()
