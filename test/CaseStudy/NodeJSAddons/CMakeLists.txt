cet_make_exec(ConfToolTest_t
  LIBRARIES
    
  artdaq-database_BasicTypes
  artdaq-database_FhiclJson
  artdaq-database_JsonDocument
  artdaq-database_ConfigurationDB
  artdaq-database_StorageProviders_MongoDB
  artdaq-database_StorageProviders_FileSystemDB
  artdaq-database_StorageProviders_UconDB  
  ${FHICLCPP}
  #${SQLITE3}
  ${CETLIB}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}  
) 


add_subdirectory(test001)
#add_subdirectory(test002)

#add_subdirectory(test003)
#add_subdirectory(test004)
#add_subdirectory(test005)

#add_subdirectory(test007)
#add_subdirectory(test008)

install_source()
