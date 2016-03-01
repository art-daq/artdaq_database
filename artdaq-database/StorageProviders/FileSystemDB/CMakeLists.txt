art_make(
  LIB_LIBRARIES
  artdaq-database_BasicTypes
  artdaq-database_JsonDocument
  artdaq-database_FhiclJson
  ${BSONCXXLIB}
  ${MONGOCXXLIB}
  ${CETLIB}
  ${Boost_FILESYSTEM_LIBRARY}    
  ${Boost_SYSTEM_LIBRARY}    
)

install_headers()
install_source()
