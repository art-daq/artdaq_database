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
  artdaq-database::SharedCommon
  artdaq-database::BasicTypes
  artdaq-database::JsonDocument
  artdaq-database::StorageProviders
  CURL::libcurl
)

install_headers()
install_source()
