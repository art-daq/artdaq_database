cet_make_exec(NAME FileSystemFhicl_t
  LIBRARIES
    
  artdaq-database_BasicTypes
  artdaq-database_DataFormats_Fhicl
  artdaq-database_DataFormats_Json
  artdaq-database_JsonDocument
  artdaq-database_StorageProviders_FileSystemDB
  fhiclcpp
  #${SQLITE3}
  cetlib
  cetlib_except
  Boost::program_options
) 

add_subdirectory(test001)
##add_subdirectory(test002) #not included into the search index
add_subdirectory(test003)
add_subdirectory(test004)

install_source()
