cet_make(
  LIBRARIES
  
  artdaq-database_DataFormats_Json
  artdaq-database_DataFormats_Fhicl
  artdaq-database_DataFormats_Xml  
  artdaq-database_JsonDocument
  artdaq-database_BasicTypes
  artdaq-database_StorageProviders_MongoDB  
  artdaq-database_StorageProviders_FileSystemDB
  artdaq-database_StorageProviders_UconDB
  ${Boost_PROGRAM_OPTIONS_LIBRARY}    
  )

install_headers()
install_source()
