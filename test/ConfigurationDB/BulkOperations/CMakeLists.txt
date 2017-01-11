cet_make_exec(BulkOperations_t
  LIBRARIES
    
  artdaq-database_BasicTypes
  
  artdaq-database_DataFormats_Fhicl
  artdaq-database_DataFormats_Json
  artdaq-database_JsonDocument
  artdaq-database_ConfigurationDB
  artdaq-database_StorageProviders_MongoDB
  artdaq-database_StorageProviders_FileSystemDB
  artdaq-database_StorageProviders_UcondDB  
  ${FHICLCPP}
  ${SQLITE3}
  ${CETLIB}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}  
) 

SET (TEST_COLLECTION_NAME "testBulkOperations" )


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

add_subdirectory(test000)

add_subdirectory(test001)
