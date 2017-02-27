execute_process( COMMAND "date" "-u" "+%m%d%Y-%H%M%S" OUTPUT_VARIABLE CONFIG_NUMBER )
string( STRIP ${CONFIG_NUMBER} CONFIG_NUMBER )

string(RANDOM LENGTH 3 ALPHABET 0123456789 RAND_NAME)
message(STATUS "RAND_NAME=${RAND_NAME}")

string(RANDOM LENGTH 3 ALPHABET 0123456789 VERSION_NUMBER)
message(STATUS "VERSION_NUMBER=${VERSION_NUMBER}")

string(RANDOM LENGTH 3 ALPHABET 0123456789 ALIAS_NUMBER)
message(STATUS "ALIAS_NUMBER=${ALIAS_NUMBER}")

set (CONFIG_NUMBER "03252016-222027")

set (TEST_CONFIG_NAME "testConfigA${CONFIG_NUMBER}")

set (TEST_ENTITY_NAME "Aggregator_topol${CONFIG_NUMBER}")

set (TEST_ALIAS_NAME "testAliasA${ALIAS_NUMBER}")

add_subdirectory(test000)
add_subdirectory(test001)
add_subdirectory(test002)
add_subdirectory(test003)
add_subdirectory(test004)
add_subdirectory(test005)

install_source()
