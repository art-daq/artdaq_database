cet_make_exec(NAME Dune35tDemo_t
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


SET (TEST_COLLECTION_NAME "testDune35tDemo" )


execute_process( COMMAND "date" "-u" "+%m%d%Y-%H%M%S" OUTPUT_VARIABLE CONFIG_NUMBER )
string( STRIP ${CONFIG_NUMBER} CONFIG_NUMBER )

string(RANDOM LENGTH 5 ALPHABET 0123456789 RAND_NAME)
message(STATUS "RAND_NAME=${RAND_NAME}")

string(RANDOM LENGTH 5 ALPHABET 0123456789 VERSION_NUMBER)
message(STATUS "VERSION_NUMBER=${VERSION_NUMBER}")

### uncomment to debug tests
#set (CONFIG_NUMBER "05202016-201808")
#set (VERSION_NUMBER "56774")

set (TEST_CONFIGURATION_NAME "testConfigA${CONFIG_NUMBER}")
SET (TEST_VERSION_NAME    versionA${VERSION_NUMBER} )

message(STATUS "TEST_CONFIGURATION_NAME=${TEST_CONFIGURATION_NAME}")
message(STATUS "TEST_VERSION_NAME=${TEST_VERSION_NAME}")
message(STATUS "CONFIG_NUMBER=${CONFIG_NUMBER}")
message(STATUS "VERSION_NUMBER=${VERSION_NUMBER}")


add_subdirectory(test001)
#add_subdirectory(test002)
add_subdirectory(test003)
add_subdirectory(test004)
add_subdirectory(test005)

install_source()
