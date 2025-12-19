SET ( TEST_NUMBER "005" )
SET ( TEST_NAME "rmalias" )


configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0051.cmp.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0051.cmp.${RAND_NAME}.json @ONLY )

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0052.cmp.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0052.cmp.${RAND_NAME}.json @ONLY )

# Register test with CTest
add_test(NAME ConfigAliasBuilder_mongo_${TEST_NAME}_${TEST_NUMBER}_01_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/ConfigurationDB_t -p mongo -o rmveralias -c artdaq_Aggregator -v version001 -a ${TEST_ALIAS_NAME} -f db -x test0051.cmp.${RAND_NAME}.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(ConfigAliasBuilder_mongo_${TEST_NAME}_${TEST_NUMBER}_01_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_MONGO}b")

# Register test with CTest
add_test(NAME ConfigAliasBuilder_filesystem_${TEST_NAME}_${TEST_NUMBER}_01_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/ConfigurationDB_t -p filesystem -o rmveralias -c artdaq_Aggregator -v version001 -a ${TEST_ALIAS_NAME} -f db -x test0052.cmp.${RAND_NAME}.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(ConfigAliasBuilder_filesystem_${TEST_NAME}_${TEST_NUMBER}_01_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}b")

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0051.flt.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0051.flt.${RAND_NAME}.json @ONLY )

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0052.flt.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0052.flt.${RAND_NAME}.json @ONLY )

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0053.cmp.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0053.cmp.${RAND_NAME}.json @ONLY )

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0054.cmp.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0054.cmp.${RAND_NAME}.json @ONLY )

# Register test with CTest
add_test(NAME conftool_mongo_${TEST_NAME}_${TEST_NUMBER}_01_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/conftool --searchquery=test0051.flt.${RAND_NAME}.json -x test0053.cmp.${RAND_NAME}.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(conftool_mongo_${TEST_NAME}_${TEST_NUMBER}_01_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_MONGO}b")

# Register test with CTest
add_test(NAME conftool_filesystem_${TEST_NAME}_${TEST_NUMBER}_01_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/conftool --searchquery=test0052.flt.${RAND_NAME}.json -x test0054.cmp.${RAND_NAME}.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(conftool_filesystem_${TEST_NAME}_${TEST_NUMBER}_01_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}b")
