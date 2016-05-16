find_ups_product( mongodb ${mongodb_product_version} )

include_directories($ENV{MONGODB_FQ_DIR}/include/bsoncxx/v0.3)
include_directories($ENV{MONGODB_FQ_DIR}/include/mongocxx/v0.3)

find_library(BSONCXXLIB NAMES bsoncxx HINTS $ENV{MONGODB_FQ_DIR}/lib )
find_library(MONGOCXXLIB NAMES mongocxx HINTS $ENV{MONGODB_FQ_DIR}/lib )

 art_make_exec(ManageConfigs_t
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
  ${CETLIB}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}  
) 

SET (TEST_COLLECTION_NAME "testManageConfigs" )


execute_process( COMMAND "date" "-u" "+%m%d%Y-%H%M%S" OUTPUT_VARIABLE CONFIG_NUMBER )
string( STRIP ${CONFIG_NUMBER} CONFIG_NUMBER )

string(RANDOM LENGTH 3 ALPHABET 0123456789 RAND_NAME)
message(STATUS "RAND_NAME=${RAND_NAME}")

string(RANDOM LENGTH 5 ALPHABET 0123456789 VERSION_NUMBER)
message(STATUS "VERSION_NUMBER=${VERSION_NUMBER}")

### uncomment to debug tests
#set (CONFIG_NUMBER "05132016-170247")
#set (VERSION_NUMBER "77611")

set (TEST_CONFIGURATION_NAME "testConfigA${CONFIG_NUMBER}")
SET (TEST_VERSION_NAME    versionA${VERSION_NUMBER} )

add_subdirectory(test000)
add_subdirectory(test001)
#add_subdirectory(test011)

#add_subdirectory(test002)
#add_subdirectory(test012)

#add_subdirectory(test003)
#add_subdirectory(test013)

#add_subdirectory(test004)
#add_subdirectory(test014)


