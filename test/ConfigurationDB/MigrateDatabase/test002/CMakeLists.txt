add_executable(MigrateDatabase_t MigrateDatabase_t.cc)

target_link_libraries(MigrateDatabase_t PRIVATE
  artdaq-database::BasicTypes
  artdaq-database::DataFormats_Fhicl
  artdaq-database::DataFormats_Json
  artdaq-database::JsonDocument
  artdaq-database::ConfigurationDB
  artdaq-database::StorageProviders_MongoDB
  artdaq-database::StorageProviders_FileSystemDB
  artdaq-database::StorageProviders_UconDB
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
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/MigrateDatabase_t --uri=${ARTDAQ_DATABASE_URI}
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_dependencies(MigrateDatabase_t untar_MigrateDatabase )
