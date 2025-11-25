SET ( TEST_NUMBER "002" )

SET ( TEST_NAME "search1" )


string(RANDOM LENGTH 5 ALPHABET 0123456789 RAND_NAME)
configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.fcl ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.src.${RAND_NAME}.fcl @ONLY )
configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.fcl ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.cmp.${RAND_NAME}.fcl @ONLY )
configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.opts.json ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.opts.${RAND_NAME}.json @ONLY )
# Register test with CTest
add_test(NAME MongoFhicl_${TEST_NAME}_${TEST_NUMBER}_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/MongoFhicl_t -s test${TEST_NUMBER}.src.${RAND_NAME}.fcl -c test${TEST_NUMBER}.cmp.${RAND_NAME}.fcl -t ${TEST_NAME} -o test${TEST_NUMBER}.opts.${RAND_NAME}.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
