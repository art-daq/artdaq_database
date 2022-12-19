cet_make_exec(NAME ValidateJson_t
  LIBRARIES
    
  artdaq-database_BasicTypes
  artdaq-database_DataFormats_Fhicl
  artdaq-database_DataFormats_Json
  artdaq-database_JsonDocument
  artdaq-database_ConfigurationDB
  artdaq-database_StorageProviders_MongoDB
  artdaq-database_StorageProviders_FileSystemDB
  artdaq-database_StorageProviders_UconDB  
  fhiclcpp
  #${SQLITE3}
  cetlib
  Boost::program_options
)

SET ( TEST_NAME "ValidateJson" )

cet_test(${TEST_NAME}_t HANDBUILT  TEST_EXEC ValidateJson_t 
  DATAFILES 
  TEST_ARGS  )
