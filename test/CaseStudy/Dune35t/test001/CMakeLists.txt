add_executable(ImportDune35tData_t ImportDune35tData_t.cc)

target_link_libraries(ImportDune35tData_t PRIVATE
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


SET (TEST_COLLECTION_NAME "testDune35tDemo" )


execute_process( COMMAND "date" "-u" "+%m%d%Y-%H%M%S" OUTPUT_VARIABLE CONFIG_NUMBER )
string( STRIP ${CONFIG_NUMBER} CONFIG_NUMBER )

string(RANDOM LENGTH 5 ALPHABET 0123456789 RAND_NAME)
message(STATUS "RAND_NAME=${RAND_NAME}")

string(RANDOM LENGTH 5 ALPHABET 0123456789 VERSION_NUMBER)
message(STATUS "VERSION_NUMBER=${VERSION_NUMBER}")

### uncomment to debug tests
#set (CONFIG_NUMBER "05172016-193725")
#set (VERSION_NUMBER "16185")

set (TEST_CONFIGURATION_NAME "testConfigA${CONFIG_NUMBER}")
SET (TEST_VERSION_NAME    versionA${VERSION_NUMBER} )

SET ( TEST_NUMBER "001" )
SET ( TEST_NAME "Dune35t_bulkimport" )

SET ( TEST_DBPROVIDER "filesystem" )
# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/ImportDune35tData_t -o store -p ${TEST_DBPROVIDER} -f fhicl -v ${TEST_VERSION_NAME}-${TEST_NUMBER} -g ${TEST_CONFIGURATION_NAME}-${TEST_NUMBER} -s "${CMAKE_CURRENT_SOURCE_DIR}/../seeddata"
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

SET ( TEST_DBPROVIDER "mongo" )
# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/ImportDune35tData_t -o store -p ${TEST_DBPROVIDER} -f fhicl -v ${TEST_VERSION_NAME}-${TEST_NUMBER} -g ${TEST_CONFIGURATION_NAME}-${TEST_NUMBER} -s "${CMAKE_CURRENT_SOURCE_DIR}/../seeddata"
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}")

install_source()
