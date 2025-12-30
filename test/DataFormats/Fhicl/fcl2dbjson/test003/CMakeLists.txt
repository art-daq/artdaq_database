SET ( TEST_NAME "RoundConvert_Fhicl2DBJson" )

SET ( TEST_NUMBER "001" )

# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.fcl COPYONLY)

# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_NUMBER}_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/ConvertFhicl_t -s test${TEST_NUMBER}.fcl -c test${TEST_NUMBER}.fcl -t ${TEST_NAME}
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})


SET ( TEST_NUMBER "002" )

# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.fcl COPYONLY)

# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_NUMBER}_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/ConvertFhicl_t -s test${TEST_NUMBER}.fcl -c test${TEST_NUMBER}.fcl -t ${TEST_NAME}
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})


SET ( TEST_NUMBER "003" )

# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.fcl COPYONLY)

# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_NUMBER}_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/ConvertFhicl_t -s test${TEST_NUMBER}.fcl -c test${TEST_NUMBER}.fcl -t ${TEST_NAME}
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

SET ( TEST_NUMBER "004" )

# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.fcl COPYONLY)

# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_NUMBER}_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/ConvertFhicl_t -s test${TEST_NUMBER}.fcl -c test${TEST_NUMBER}.fcl -t ${TEST_NAME}
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

SET ( TEST_NUMBER "005" )

# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.fcl COPYONLY)

# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_NUMBER}_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/ConvertFhicl_t -s test${TEST_NUMBER}.fcl -c test${TEST_NUMBER}.fcl -t ${TEST_NAME}
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

SET ( TEST_NUMBER "006" )

# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.fcl COPYONLY)

# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_NUMBER}_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/ConvertFhicl_t -s test${TEST_NUMBER}.fcl -c test${TEST_NUMBER}.fcl -t ${TEST_NAME}
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

SET ( TEST_NUMBER "007" )

# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.fcl COPYONLY)

# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_NUMBER}_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/ConvertFhicl_t -s test${TEST_NUMBER}.fcl -c test${TEST_NUMBER}.fcl -t ${TEST_NAME}
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

SET ( TEST_NUMBER "008" )

# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.fcl COPYONLY)

# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_NUMBER}_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/ConvertFhicl_t -s test${TEST_NUMBER}.fcl -c test${TEST_NUMBER}.fcl -t ${TEST_NAME}
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

SET ( TEST_NUMBER "009" )

# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.fcl COPYONLY)

# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_NUMBER}_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/ConvertFhicl_t -s test${TEST_NUMBER}.fcl -c test${TEST_NUMBER}.fcl -t ${TEST_NAME}
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

SET ( TEST_NUMBER "010" )

# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.fcl COPYONLY)

# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_NUMBER}_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/ConvertFhicl_t -s test${TEST_NUMBER}.fcl -c test${TEST_NUMBER}.fcl -t ${TEST_NAME}
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

SET ( TEST_NUMBER "011" )

# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.fcl COPYONLY)

# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_NUMBER}_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/ConvertFhicl_t -s test${TEST_NUMBER}.fcl -c test${TEST_NUMBER}.fcl -t ${TEST_NAME}
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

SET ( TEST_NUMBER "012" )

# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.fcl COPYONLY)

# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_NUMBER}_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/ConvertFhicl_t -s test${TEST_NUMBER}.fcl -c test${TEST_NUMBER}.fcl -t ${TEST_NAME}
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

SET ( TEST_NUMBER "013" )

# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.fcl COPYONLY)

# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_NUMBER}_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/ConvertFhicl_t -s test${TEST_NUMBER}.fcl -c test${TEST_NUMBER}.fcl -t ${TEST_NAME}
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

SET ( TEST_NUMBER "014" )

# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.fcl COPYONLY)

# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_NUMBER}_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/ConvertFhicl_t -s test${TEST_NUMBER}.fcl -c test${TEST_NUMBER}.fcl -t ${TEST_NAME}
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

SET ( TEST_NUMBER "015" )

# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.fcl COPYONLY)

# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_NUMBER}_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/ConvertFhicl_t -s test${TEST_NUMBER}.fcl -c test${TEST_NUMBER}.fcl -t ${TEST_NAME}
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

SET ( TEST_NUMBER "016" )

# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.fcl
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.fcl COPYONLY)

# Register test with CTest
add_test(NAME ${TEST_NAME}_${TEST_NUMBER}_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/ConvertFhicl_t -s test${TEST_NUMBER}.fcl -c test${TEST_NUMBER}.fcl -t ${TEST_NAME}
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
