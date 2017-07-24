cet_make(
  LIBRARIES
  artdaq-database_SharedCommon
  artdaq-database_BasicTypes
  artdaq-database_JsonDocument
  artdaq-database_StorageProviders
  ${BSONCXXLIB}
  ${MONGOCXXLIB}
)

install_headers()
install_source()
