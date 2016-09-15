cet_make_exec(ValidateJson_t
  LIBRARIES
  artdaq-database_StackTrace  
  artdaq-database_BasicTypes
  artdaq-database_DataFormats_common
  artdaq-database_DataFormats_Fhicl
  artdaq-database_DataFormats_Json
  artdaq-database_JsonDocument
  artdaq-database_ConfigurationDB
  artdaq-database_StorageProviders_MongoDB
  artdaq-database_StorageProviders_FileSystemDB
  ${FHICLCPP}
  ${SQLITE3}
  ${CETLIB}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}  
)

SET ( TEST_NAME "ValidateJson" )

cet_test(${TEST_NAME}_t HANDBUILT  TEST_EXEC ValidateJson_t 
  DATAFILES 
  TEST_ARGS  )
