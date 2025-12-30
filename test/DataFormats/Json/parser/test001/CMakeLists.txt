SET ( TEST_NAME "Readjson" )

SET ( TEST_NUMBER "001" )
# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.json COPYONLY)

# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_NUMBER}_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/readjson_t -c test${TEST_NUMBER}.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

SET ( TEST_NUMBER "002" )
# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.json COPYONLY)

# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_NUMBER}_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/readjson_t -c test${TEST_NUMBER}.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

SET ( TEST_NUMBER "003" )
# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.json COPYONLY)

# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_NUMBER}_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/readjson_t -c test${TEST_NUMBER}.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

SET ( TEST_NUMBER "004" )
# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.json COPYONLY)

# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_NUMBER}_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/readjson_t -c test${TEST_NUMBER}.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

SET ( TEST_NUMBER "005" )
# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.json COPYONLY)

# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_NUMBER}_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/readjson_t -c test${TEST_NUMBER}.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})


SET ( TEST_NUMBER "006" )
# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.json COPYONLY)

# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_NUMBER}_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/readjson_t -c test${TEST_NUMBER}.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

SET ( TEST_NUMBER "007" )
# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.json COPYONLY)

# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_NUMBER}_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/readjson_t -c test${TEST_NUMBER}.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
