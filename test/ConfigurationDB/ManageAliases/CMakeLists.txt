cet_make_exec(ConfigAliasBuilder_t
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
  ${FHICLCPP}
  ${SQLITE3}
  ${CETLIB}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}  
) 

execute_process( COMMAND "date" "-u" "+%m%d%Y-%H%M%S" OUTPUT_VARIABLE CONFIG_NUMBER )
string( STRIP ${CONFIG_NUMBER} CONFIG_NUMBER )

string(RANDOM LENGTH 3 ALPHABET 0123456789 RAND_NAME)
message(STATUS "RAND_NAME=${RAND_NAME}")

string(RANDOM LENGTH 3 ALPHABET 0123456789 VERSION_NUMBER)
message(STATUS "VERSION_NUMBER=${VERSION_NUMBER}")

set (CONFIG_NUMBER "03252016-222027")

set (TEST_CONFIG_NAME "testConfigA${CONFIG_NUMBER}")

set (TEST_ENTITY_NAME "Aggregator_topol${CONFIG_NUMBER}")

add_subdirectory(test000)
#add_subdirectory(test001)
#add_subdirectory(test002)
#add_subdirectory(test003)

install_source()
