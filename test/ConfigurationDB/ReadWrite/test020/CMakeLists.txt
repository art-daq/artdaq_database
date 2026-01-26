SET ( TEST_NAME "ReadWrite_store" )

SET ( TEST_NUMBER "020" )

SET ( TEST_DBPROVIDER "filesystem" )
string(RANDOM LENGTH 5 ALPHABET 0123456789 RAND_NAME)
configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test011.src.json ${CMAKE_CURRENT_BINARY_DIR}/test011.src.${RAND_NAME}.json @ONLY )
configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test011.cmp.json ${CMAKE_CURRENT_BINARY_DIR}/test011.cmp.${RAND_NAME}.json @ONLY )
configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test011.flt.json ${CMAKE_CURRENT_BINARY_DIR}/test011.flt.${RAND_NAME}.json @ONLY )
# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/ConfigurationDB_t --searchquery=test011.flt.${RAND_NAME}.json -o store -s test011.src.${RAND_NAME}.json -x test011.cmp.${RAND_NAME}.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}")
