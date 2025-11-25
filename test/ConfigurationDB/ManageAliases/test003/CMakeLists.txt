SET ( TEST_NUMBER "003" )
SET ( TEST_NAME "findentities" )


configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0031.cmp.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0031.cmp.${RAND_NAME}.json @ONLY )

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0032.cmp.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0032.cmp.${RAND_NAME}.json @ONLY )

# Register test with CTest
add_test(NAME ConfigAliasBuilder_mongo_${TEST_NAME}_${TEST_NUMBER}_01_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/ConfigurationDB_t -p mongo -o findentities -f gui -x test0031.cmp.${RAND_NAME}.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

# Register test with CTest
add_test(NAME ConfigAliasBuilder_filesystem_${TEST_NAME}_${TEST_NUMBER}_01_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/ConfigurationDB_t -p filesystem -o findentities -f gui -x test0032.cmp.${RAND_NAME}.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(ConfigAliasBuilder_filesystem_${TEST_NAME}_${TEST_NUMBER}_01_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_MONGO}b")

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0031.flt.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0031.flt.${RAND_NAME}.json @ONLY )

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0032.flt.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0032.flt.${RAND_NAME}.json @ONLY )

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0033.cmp.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0033.cmp.${RAND_NAME}.json @ONLY )

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0034.cmp.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0034.cmp.${RAND_NAME}.json @ONLY )

# Register test with CTest
add_test(NAME conftool_mongo_${TEST_NAME}_${TEST_NUMBER}_01_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/conftool --searchquery=test0031.flt.${RAND_NAME}.json -x test0033.cmp.${RAND_NAME}.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(conftool_mongo_${TEST_NAME}_${TEST_NUMBER}_01_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}b")

# Register test with CTest
add_test(NAME conftool_filesystem_${TEST_NAME}_${TEST_NUMBER}_01_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/conftool --searchquery=test0032.flt.${RAND_NAME}.json -x test0034.cmp.${RAND_NAME}.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(conftool_filesystem_${TEST_NAME}_${TEST_NUMBER}_01_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}b")