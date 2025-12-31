SET ( TEST_NUMBER "003" )

SET ( TEST_NAME "update" )

# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.src.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.cmp.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.update.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.update.json COPYONLY)

# Register test with CTest
add_test(NAME UcondJson_${TEST_NAME}_${TEST_NUMBER}_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/UcondJson_t -s test${TEST_NUMBER}.src.json -c test${TEST_NUMBER}.cmp.json -t ${TEST_NAME}  -o test${TEST_NUMBER}.update.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
