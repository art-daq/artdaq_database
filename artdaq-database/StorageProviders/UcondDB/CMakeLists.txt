cet_make(
  LIBRARIES
  artdaq-database_BasicTypes
  artdaq-database_JsonDocument
  artdaq-database_StorageProviders
  ${BSONCXXLIB}
  ${MONGOCXXLIB}
  ${CETLIB}
)

install_headers()
install_source()
