SET ( TEST_NUMBER "004" )

SET ( TEST_NAME "search2" )

# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.src.fcl COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.cmp.fcl COPYONLY)

# Register test with CTest
add_test(NAME FileSystemFhicl_${TEST_NAME}_${TEST_NUMBER}_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/FileSystemFhicl_t -s test${TEST_NUMBER}.src.fcl -c test${TEST_NUMBER}.cmp.fcl -t ${TEST_NAME})
set_tests_properties(FileSystemFhicl_${TEST_NAME}_${TEST_NUMBER}_t PROPERTIES
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}")
