cet_make_exec(NAME MigrateDatabase_t
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

SET ( TEST_NAME "MigrateDatabase" )

set (ARTDAQ_DATABASE_URI "filesystemdb:///${CMAKE_BINARY_DIR}/databases/filesystemdb/test_dbg")

execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/databases/filesystemdb/test_dbg/)

add_custom_target(untar_MigrateDatabase ALL
WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/databases/filesystemdb/test_dbg/"
COMMAND ${CMAKE_COMMAND} -E tar "xfz" "${CMAKE_CURRENT_SOURCE_DIR}/filesystemdb.taz")

SET ( TEST_NUMBER "002" )
# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_NUMBER}_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/MigrateDatabase_t --uri=${ARTDAQ_DATABASE_URI}
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_dependencies(untar_MigrateDatabase MigrateDatabase_t)
