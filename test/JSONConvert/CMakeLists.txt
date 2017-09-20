cet_make_exec(JSONConvert_t
  LIBRARIES
    
  artdaq-database_BasicTypes
  artdaq-database_FhiclJson
  artdaq-database_JsonDocument
  artdaq-database_ConfigurationDB
  artdaq-database_StorageProviders_MongoDB
  artdaq-database_StorageProviders_FileSystemDB
  artdaq-database_StorageProviders_UconDB
  ${FHICLCPP}
  ${CETLIB}
  #${SQLITE3}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}  
) 


add_subdirectory(test001)
install_source()
