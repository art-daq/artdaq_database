SET ( TEST_NUMBER "001" )

SET ( TEST_NAME "insert" )

string(RANDOM LENGTH 5 ALPHABET 0123456789 RAND_NAME)

# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.src.fcl COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.cmp.fcl COPYONLY)

# Register test with CTest
add_test(NAME MongoFhicl_${TEST_NAME}_${TEST_NUMBER}_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/MongoFhicl_t -s test${TEST_NUMBER}.src.fcl -c test${TEST_NUMBER}.cmp.fcl -t ${TEST_NAME} -n testFHICL_${RAND_NAME}
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
