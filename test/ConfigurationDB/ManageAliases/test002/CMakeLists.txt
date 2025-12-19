SET ( TEST_NUMBER "002" )
SET ( TEST_NAME "addconfig" )

execute_process( COMMAND "date" "-u" "+%m%d%Y-%H%M%S" OUTPUT_VARIABLE CONFIG_NUMBER )
string( STRIP ${CONFIG_NUMBER} CONFIG_NUMBER )
#set (CONFIG_NUMBER "03252016-222027")
set (TEST_CONFIGURATION_NAME "testConfigA${CONFIG_NUMBER}")

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0021.cmp.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0021.cmp.${RAND_NAME}.json @ONLY )

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0022.cmp.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0022.cmp.${RAND_NAME}.json @ONLY )

# Register test with CTest
add_test(NAME ConfigAliasBuilder_mongo_${TEST_NAME}_${TEST_NUMBER}_01_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/ConfigurationDB_t -p mongo -o addconfig -f gui -e ${TEST_ENTITY_NAME} -c artdaq_Aggregator -g ${TEST_CONFIGURATION_NAME} -v version004 -x test0021.cmp.${RAND_NAME}.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(ConfigAliasBuilder_mongo_${TEST_NAME}_${TEST_NUMBER}_01_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_MONGO}b")

# Register test with CTest
add_test(NAME ConfigAliasBuilder_filesystem_${TEST_NAME}_${TEST_NUMBER}_01_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/ConfigurationDB_t -p filesystem -o addconfig  -f gui -e ${TEST_ENTITY_NAME} -c artdaq_Aggregator -g ${TEST_CONFIGURATION_NAME} -v version004 -x test0022.cmp.${RAND_NAME}.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(ConfigAliasBuilder_filesystem_${TEST_NAME}_${TEST_NUMBER}_01_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}b")


set (TEST_CONFIGURATION_NAME "testConfigB${CONFIG_NUMBER}")

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0021.flt.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0021.flt.${RAND_NAME}.json @ONLY )

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0022.flt.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0022.flt.${RAND_NAME}.json @ONLY )

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0023.cmp.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0023.cmp.${RAND_NAME}.json @ONLY )

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0024.cmp.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0024.cmp.${RAND_NAME}.json @ONLY )

# Register test with CTest
add_test(NAME conftool_mongo_${TEST_NAME}_${TEST_NUMBER}_01_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/conftool --searchquery=test0021.flt.${RAND_NAME}.json -x test0023.cmp.${RAND_NAME}.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(conftool_mongo_${TEST_NAME}_${TEST_NUMBER}_01_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_MONGO}b")

# Register test with CTest
add_test(NAME conftool_filesystem_${TEST_NAME}_${TEST_NUMBER}_01_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/conftool --searchquery=test0022.flt.${RAND_NAME}.json -x test0024.cmp.${RAND_NAME}.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(conftool_filesystem_${TEST_NAME}_${TEST_NUMBER}_01_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}b")
