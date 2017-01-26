FIND_PACKAGE(CURL)

IF(NOT CURL_FOUND)
  MESSAGE(FATAL_ERROR "Could not find the CURL library.")
ENDIF()

INCLUDE_DIRECTORIES(${CURL_INCLUDE_DIR})

cet_make(
  LIBRARIES
  artdaq-database_SharedCommon
  artdaq-database_BasicTypes
  artdaq-database_JsonDocument
  artdaq-database_StorageProviders
  ${CURL_LIBRARIES}
)

install_headers()
install_source()
