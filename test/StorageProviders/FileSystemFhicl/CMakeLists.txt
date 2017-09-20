cet_make_exec(FileSystemFhicl_t
  LIBRARIES
    
  artdaq-database_BasicTypes
  artdaq-database_DataFormats_Fhicl
  artdaq-database_DataFormats_Json
  artdaq-database_JsonDocument
  artdaq-database_StorageProviders_FileSystemDB
  ${FHICLCPP}
  #${SQLITE3}
  ${CETLIB}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}    
) 

add_subdirectory(test001)
##add_subdirectory(test002) #not included into the search index
add_subdirectory(test003)
add_subdirectory(test004)

install_source()
