SET ( TEST_NUMBER "004" )
SET ( TEST_NAME "addalias" )


configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0041.cmp.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0041.cmp.${RAND_NAME}.json @ONLY )

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0042.cmp.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0042.cmp.${RAND_NAME}.json @ONLY )

# Register test with CTest
add_test(NAME ConfigAliasBuilder_mongo_${TEST_NAME}_${TEST_NUMBER}_01_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/ConfigurationDB_t -p mongo -o addveralias -c artdaq_Aggregator -v version001 -a ${TEST_ALIAS_NAME} -f db -x test0041.cmp.${RAND_NAME}.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

# Register test with CTest
add_test(NAME ConfigAliasBuilder_filesystem_${TEST_NAME}_${TEST_NUMBER}_01_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/ConfigurationDB_t -p filesystem -o addveralias -c artdaq_Aggregator -v version001 -a ${TEST_ALIAS_NAME} -f db -x test0042.cmp.${RAND_NAME}.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(ConfigAliasBuilder_filesystem_${TEST_NAME}_${TEST_NUMBER}_01_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_MONGO}b")

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0041.flt.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0041.flt.${RAND_NAME}.json @ONLY )

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0042.flt.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0042.flt.${RAND_NAME}.json @ONLY )

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0043.cmp.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0043.cmp.${RAND_NAME}.json @ONLY )

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0044.cmp.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0044.cmp.${RAND_NAME}.json @ONLY )

# Register test with CTest
add_test(NAME conftool_mongo_${TEST_NAME}_${TEST_NUMBER}_01_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/conftool --searchquery=test0041.flt.${RAND_NAME}.json -x test0043.cmp.${RAND_NAME}.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(conftool_mongo_${TEST_NAME}_${TEST_NUMBER}_01_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}b")

# Register test with CTest
add_test(NAME conftool_filesystem_${TEST_NAME}_${TEST_NUMBER}_01_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/conftool --searchquery=test0042.flt.${RAND_NAME}.json -x test0044.cmp.${RAND_NAME}.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(conftool_filesystem_${TEST_NAME}_${TEST_NUMBER}_01_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}b")