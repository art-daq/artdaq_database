SET ( TEST_NUMBER "002" )
SET ( TEST_NAME "json2conf" )

# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.conf
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.conf COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.json COPYONLY)

# Register test with CTest
add_test(NAME CONFConvert_${TEST_NAME}_${TEST_NUMBER}_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/CONFConvert_t -s test${TEST_NUMBER}.json -c test${TEST_NUMBER}.conf -t ${TEST_NAME}
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
