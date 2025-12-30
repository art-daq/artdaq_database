SET ( TEST_NUMBER "003" )

SET ( TEST_NAME "update" )

# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.xml
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.src.xml COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.xml
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.cmp.xml COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.update.xml
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.update.xml COPYONLY)

# Register test with CTest
add_test(NAME FileSystemXml_${TEST_NAME}_${TEST_NUMBER}_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/FileSystemXml_t -s test${TEST_NUMBER}.src.xml -c test${TEST_NUMBER}.cmp.xml -t ${TEST_NAME}  -o test${TEST_NUMBER}.update.xml)
set_tests_properties(FileSystemXml_${TEST_NAME}_${TEST_NUMBER}_t PROPERTIES
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}")
