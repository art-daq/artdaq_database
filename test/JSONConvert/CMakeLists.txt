cet_make_exec(NAME JSONConvert_t
  LIBRARIES
    
  artdaq-database_BasicTypes
  artdaq-database_FhiclJson
  artdaq-database_JsonDocument
  artdaq-database_ConfigurationDB
  artdaq-database_StorageProviders_MongoDB
  artdaq-database_StorageProviders_FileSystemDB
  artdaq-database_StorageProviders_UconDB
  fhiclcpp
  cetlib
  cetlib_except
  Boost::program_options
) 


add_subdirectory(test001)
install_source()
