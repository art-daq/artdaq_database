add_executable(MigrateDocument_t MigrateDocument_t.cc)

target_link_libraries(MigrateDocument_t PRIVATE
  artdaq-database_BasicTypes
  artdaq-database_DataFormats_Fhicl
  artdaq-database_DataFormats_Json
  artdaq-database_JsonDocument
  artdaq-database_ConfigurationDB
  artdaq-database_StorageProviders_MongoDB
  artdaq-database_StorageProviders_FileSystemDB
  artdaq-database_StorageProviders_UconDB
  Boost::program_options
)

SET ( TEST_NAME "MigrateDocument" )

SET ( TEST_NUMBER "001" )
string(RANDOM LENGTH 5 ALPHABET 0123456789 RAND_NAME)
configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test001.src.json ${CMAKE_CURRENT_BINARY_DIR}/test001.src.${RAND_NAME}.json @ONLY )
configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test001.cmp.json ${CMAKE_CURRENT_BINARY_DIR}/test001.cmp.${RAND_NAME}.json @ONLY )
# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_NUMBER}_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/MigrateDocument_t -s test001.src.${RAND_NAME}.json -x test001.cmp.${RAND_NAME}.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
