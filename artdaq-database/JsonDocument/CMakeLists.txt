cet_make(
  LIBRARIES
  artdaq-database_DataFormats_common
  artdaq-database_DataFormats_Json
  artdaq-database_StorageProviders_MongoDB  
  ${BSONCXXLIB}
  ${CETLIB}  
  )

install_headers()
install_source()
