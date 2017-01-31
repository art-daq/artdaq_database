cet_make_exec(ReadWrite_t
  LIBRARIES
    
  artdaq-database_BasicTypes
  
  artdaq-database_DataFormats_Fhicl
  artdaq-database_DataFormats_Json
  artdaq-database_JsonDocument
  artdaq-database_ConfigurationDB
  artdaq-database_StorageProviders_MongoDB
  artdaq-database_StorageProviders_FileSystemDB
  artdaq-database_StorageProviders_UconDB  
  ${FHICLCPP}
  ${SQLITE3}
  ${CETLIB}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}  
) 

SET (TEST_COLLECTION_NAME "testReadWrite" )


execute_process( COMMAND "date" "-u" "+%m%d%Y-%H%M%S" OUTPUT_VARIABLE CONFIG_NUMBER )
string( STRIP ${CONFIG_NUMBER} CONFIG_NUMBER )

string(RANDOM LENGTH 3 ALPHABET 0123456789 RAND_NAME)
message(STATUS "RAND_NAME=${RAND_NAME}")

string(RANDOM LENGTH 5 ALPHABET 0123456789 VERSION_NUMBER)
message(STATUS "VERSION_NUMBER=${VERSION_NUMBER}")

set (CONFIG_NUMBER "03172016-174859")
set (TEST_CONFIG_NAME "testConfigA${CONFIG_NUMBER}")

SET (TEST_VERSION_NAME    versionA${VERSION_NUMBER} )
add_subdirectory(test001)
add_subdirectory(test003)

SET (TEST_VERSION_NAME    versionB${VERSION_NUMBER} )
add_subdirectory(test002)
add_subdirectory(test004)

SET (TEST_VERSION_NAME    versionC${VERSION_NUMBER} )
add_subdirectory(test011)
add_subdirectory(test013)

SET (TEST_VERSION_NAME    versionD${VERSION_NUMBER} )
add_subdirectory(test012)
add_subdirectory(test014)

SET (TEST_VERSION_NAME    versionZ${VERSION_NUMBER} )
#add_subdirectory(test020)
#add_subdirectory(test021)

