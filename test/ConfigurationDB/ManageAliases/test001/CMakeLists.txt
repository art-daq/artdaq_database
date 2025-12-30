SET ( TEST_NUMBER "001" )
SET ( TEST_NAME "findversions" )

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0011.cmp.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0011.cmp.${RAND_NAME}.json @ONLY )

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0012.cmp.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0012.cmp.${RAND_NAME}.json @ONLY )


# Register test with CTest
add_test(NAME ConfigAliasBuilder_mongo_${TEST_NAME}_${TEST_NUMBER}_01_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/ConfigurationDB_t -p mongo -o findversions -f gui -e ${TEST_ENTITY_NAME} -c artdaq_Aggregator -x test0011.cmp.${RAND_NAME}.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(ConfigAliasBuilder_mongo_${TEST_NAME}_${TEST_NUMBER}_01_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_MONGO}b")

# Register test with CTest
add_test(NAME ConfigAliasBuilder_filesystem_${TEST_NAME}_${TEST_NUMBER}_02_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/ConfigurationDB_t -p filesystem -o findversions -f gui -e ${TEST_ENTITY_NAME} -c artdaq_Aggregator -x test0012.cmp.${RAND_NAME}.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(ConfigAliasBuilder_filesystem_${TEST_NAME}_${TEST_NUMBER}_02_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}b")

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0011.flt.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0011.flt.${RAND_NAME}.json @ONLY )

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0012.flt.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0012.flt.${RAND_NAME}.json @ONLY )

# Register test with CTest
add_test(NAME conftool_mongo_${TEST_NAME}_${TEST_NUMBER}_01_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/conftool --searchquery=test0011.flt.${RAND_NAME}.json -x test0011.cmp.${RAND_NAME}.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(conftool_mongo_${TEST_NAME}_${TEST_NUMBER}_01_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_MONGO}b")

# Register test with CTest
add_test(NAME conftool_filesystem_${TEST_NAME}_${TEST_NUMBER}_01_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/conftool --searchquery=test0012.flt.${RAND_NAME}.json -x test0012.cmp.${RAND_NAME}.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(conftool_filesystem_${TEST_NAME}_${TEST_NUMBER}_01_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}b")
