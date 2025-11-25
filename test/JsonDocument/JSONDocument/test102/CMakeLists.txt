SET ( TEST_NUMBER "102" )
SET ( TEST_NAME "insertChild" )


# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/begin-state.json
               ${CMAKE_CURRENT_BINARY_DIR}/begin-state.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/end-state.json
               ${CMAKE_CURRENT_BINARY_DIR}/end-state.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/operation-delta.json
               ${CMAKE_CURRENT_BINARY_DIR}/operation-delta.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/returned-value.json
               ${CMAKE_CURRENT_BINARY_DIR}/returned-value.json COPYONLY)

# Register test with CTest
add_test(NAME JSONDocument_${TEST_NAME}_${TEST_NUMBER}_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/JSONDocument_t -c test${TEST_NUMBER}.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
