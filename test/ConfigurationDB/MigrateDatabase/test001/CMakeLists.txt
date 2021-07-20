cet_make_exec(MigrateDocument_t
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
  ${Boost_PROGRAM_OPTIONS_LIBRARY}  
)

SET ( TEST_NAME "MigrateDocument" )

SET ( TEST_NUMBER "001" )
string(RANDOM LENGTH 5 ALPHABET 0123456789 RAND_NAME)
configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test001.src.json ${CMAKE_CURRENT_BINARY_DIR}/test001.src.${RAND_NAME}.json @ONLY )
configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test001.cmp.json ${CMAKE_CURRENT_BINARY_DIR}/test001.cmp.${RAND_NAME}.json @ONLY )
cet_test(${TEST_NAME}_${TEST_NUMBER}_t HANDBUILT  TEST_EXEC MigrateDocument_t 
  DATAFILES 
    ${CMAKE_CURRENT_BINARY_DIR}/test001.src.${RAND_NAME}.json 
    ${CMAKE_CURRENT_BINARY_DIR}/test001.cmp.${RAND_NAME}.json 
  TEST_ARGS -s test001.src.${RAND_NAME}.json -x test001.cmp.${RAND_NAME}.json )
