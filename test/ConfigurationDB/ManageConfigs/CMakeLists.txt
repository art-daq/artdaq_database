SET (TEST_COLLECTION_NAME "testManageConfigs" )


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

add_subdirectory(test000)

add_subdirectory(test001)
add_subdirectory(test011)

add_subdirectory(test002)
add_subdirectory(test012)

add_subdirectory(test003)
add_subdirectory(test013)

add_subdirectory(test004)
add_subdirectory(test014)

add_subdirectory(test005)
add_subdirectory(test015)
