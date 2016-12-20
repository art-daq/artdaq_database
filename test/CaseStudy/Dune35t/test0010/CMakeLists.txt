cet_make_exec(ExportDune35tData_t
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
  artdaq-database_StorageProviders_UcondDB
  ${FHICLCPP}
  ${CETLIB}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}  
) 


SET (TEST_COLLECTION_NAME "testDune35tDemo" )


execute_process( COMMAND "date" "-u" "+%m%d%Y-%H%M%S" OUTPUT_VARIABLE CONFIG_NUMBER )
string( STRIP ${CONFIG_NUMBER} CONFIG_NUMBER )

string(RANDOM LENGTH 3 ALPHABET 0123456789 RAND_NAME)
message(STATUS "RAND_NAME=${RAND_NAME}")

string(RANDOM LENGTH 5 ALPHABET 0123456789 VERSION_NUMBER)
message(STATUS "VERSION_NUMBER=${VERSION_NUMBER}")

### uncomment to debug tests
#set (CONFIG_NUMBER "05172016-193725")
#set (VERSION_NUMBER "16185")

set (TEST_CONFIGURATION_NAME "testConfigA${CONFIG_NUMBER}")
SET (TEST_VERSION_NAME    versionA${VERSION_NUMBER} )

SET ( TEST_NUMBER "001" )
SET ( TEST_NAME "Dune35t_bulkexport" )

SET ( TEST_DBPROVIDER "filesystem" )
cet_test_env("ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}")
cet_test(${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_t HANDBUILT  TEST_EXEC ExportDune35tData_t 
  DATAFILES 
  TEST_ARGS -o store -d ${TEST_DBPROVIDER} -f fhicl -v ${TEST_VERSION_NAME}-${TEST_NUMBER} -g ${TEST_CONFIGURATION_NAME}-${TEST_NUMBER} -s "${CMAKE_CURRENT_SOURCE_DIR}/../seeddata" )

SET ( TEST_DBPROVIDER "mongo" )
cet_test_env("ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_MONGO}")
cet_test(${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_t HANDBUILT  TEST_EXEC ExportDune35tData_t 
  DATAFILES 
  TEST_ARGS -o store -d ${TEST_DBPROVIDER} -f fhicl -v ${TEST_VERSION_NAME}-${TEST_NUMBER} -g ${TEST_CONFIGURATION_NAME}-${TEST_NUMBER} -s "${CMAKE_CURRENT_SOURCE_DIR}/../seeddata" )
  
install_source()
