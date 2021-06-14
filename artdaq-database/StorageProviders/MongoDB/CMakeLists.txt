cet_make(
  LIBRARIES
  artdaq-database_SharedCommon
  artdaq-database_BasicTypes
  artdaq-database_JsonDocument
  artdaq-database_StorageProviders
  ${BSONCXXLIB}
  ${MONGOCXXLIB}
  ${CETLIB_EXCEPT}
)

add_dependencies(artdaq-database_StorageProviders_MongoDB  mongo-cxx-driver)

install_headers()
install_source()
