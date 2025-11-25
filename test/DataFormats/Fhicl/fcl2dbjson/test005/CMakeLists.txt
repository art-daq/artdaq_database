SET ( TEST_NAME "Convert_Fhicl2DBJson" )

SET ( TEST_NUMBER "006" )
# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.fcl COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.json COPYONLY)

# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_NUMBER}_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/ConvertFhicl_t -s test${TEST_NUMBER}.fcl -c test${TEST_NUMBER}.json -t ${TEST_NAME}
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
