cet_make_exec(MigrateDatabase_t
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

SET ( TEST_NAME "MigrateDatabase" )

set (ARTDAQ_DATABASE_URI "filesystemdb://${CMAKE_BINARY_DIR}/databases/filesystemdb/test_dbg")

execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/databases/filesystemdb/test_dbg/)

add_custom_target(untar_MigrateDatabase ALL 
WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/databases/filesystemdb/test_dbg/"
COMMAND ${CMAKE_COMMAND} -E tar "xfz" "${CMAKE_CURRENT_SOURCE_DIR}/filesystemdb.taz")

SET ( TEST_NUMBER "002" )
cet_test(${TEST_NAME}_${TEST_NUMBER}_t HANDBUILT  TEST_EXEC MigrateDatabase_t 
  DATAFILES
  TEST_ARGS --uri=${ARTDAQ_DATABASE_URI} )
 
add_dependencies(untar_MigrateDatabase MigrateDatabase_t)
