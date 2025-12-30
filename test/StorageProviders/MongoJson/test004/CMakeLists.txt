SET ( TEST_NUMBER "004" )

SET ( TEST_NAME "search2" )

# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.src.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.cmp.json COPYONLY)

# Register test with CTest
add_test(NAME MongoJson_${TEST_NAME}_${TEST_NUMBER}_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/MongoJson_t -s test${TEST_NUMBER}.src.json -c test${TEST_NUMBER}.cmp.json -t ${TEST_NAME}
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
