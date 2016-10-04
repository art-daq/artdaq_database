cet_make(
  LIBRARIES
  artdaq-database_BasicTypes
  artdaq-database_JsonDocument
  artdaq-database_StorageProviders
  ${CETLIB}
  ${Boost_FILESYSTEM_LIBRARY}    
  ${Boost_SYSTEM_LIBRARY}    
  ${Boost_REGEX_LIBRARY}    
)

install_headers()
install_source()
