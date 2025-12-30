SET ( TEST_NUMBER "001" )
SET ( TEST_NAME "xml2json" )

# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.xml
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.xml COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.json COPYONLY)

# Register test with CTest
add_test(NAME XMLConvert_${TEST_NAME}_${TEST_NUMBER}_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/XMLConvert_t -s test${TEST_NUMBER}.xml -c test${TEST_NUMBER}.json -t ${TEST_NAME}
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
