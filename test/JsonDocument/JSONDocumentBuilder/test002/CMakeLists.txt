SET ( TEST_NUMBER "002" )
SET ( TEST_NAME "buildDocument" )


# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/begin-state.json
               ${CMAKE_CURRENT_BINARY_DIR}/begin-state.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/end-state.json
               ${CMAKE_CURRENT_BINARY_DIR}/end-state.json COPYONLY)

# Register test with CTest
add_test(NAME JSONDocumentBuilder_${TEST_NAME}_${TEST_NUMBER}_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/JSONDocumentBuilder_t -c test${TEST_NUMBER}.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
