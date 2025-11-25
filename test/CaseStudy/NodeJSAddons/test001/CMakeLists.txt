SET ( TEST_NUMBER "001" )
SET ( TEST_NAME "load" )

# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.src.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.cmp.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.flt.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.flt.json COPYONLY)

# Register test with CTest
add_test(NAME ConfToolTest_${TEST_NAME}_${TEST_NUMBER}_01_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/ConfToolTest_t -s test${TEST_NUMBER}.src.json -c test${TEST_NUMBER}.src.fcl -f test${TEST_NUMBER}.flt.json -o store
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
