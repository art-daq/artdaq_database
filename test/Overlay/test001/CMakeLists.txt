add_executable(JSONOverlay_t JSONOverlay_t.cc)

target_link_libraries(JSONOverlay_t PRIVATE
  artdaq-database::DataFormats_Json
  artdaq-database::Overlay
  Boost::program_options
)

SET ( TEST_NAME "compare" )

SET ( TEST_NUMBER "001" )

SET ( SUBTEST_NUMBER "a" )
# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.cmp.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.src.json COPYONLY)

# Register test with CTest
add_test(NAME JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/JSONOverlay_t -m 3 -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

SET ( SUBTEST_NUMBER "b" )
# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.cmp.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.src.json COPYONLY)

# Register test with CTest
add_test(NAME JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/JSONOverlay_t -f -m 1 -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

SET ( TEST_NUMBER "002" )

SET ( SUBTEST_NUMBER "a" )
# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.cmp.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.src.json COPYONLY)

# Register test with CTest
add_test(NAME JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/JSONOverlay_t -f -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

SET ( SUBTEST_NUMBER "b" )
  # Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.cmp.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.src.json COPYONLY)

# Register test with CTest
add_test(NAME JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/JSONOverlay_t -m 1 -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

SET ( SUBTEST_NUMBER "c" )
  # Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.cmp.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.src.json COPYONLY)

# Register test with CTest
add_test(NAME JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/JSONOverlay_t -m 8 -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

SET ( TEST_NUMBER "003" )
SET ( SUBTEST_NUMBER "a" )
# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.cmp.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.src.json COPYONLY)

# Register test with CTest
add_test(NAME JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/JSONOverlay_t -f -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

SET ( SUBTEST_NUMBER "b" )
# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.cmp.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.src.json COPYONLY)

# Register test with CTest
add_test(NAME JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/JSONOverlay_t -m 8 -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

SET ( TEST_NUMBER "004" )
SET ( SUBTEST_NUMBER "a" )
# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.cmp.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.src.json COPYONLY)

# Register test with CTest
add_test(NAME JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/JSONOverlay_t -f -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

SET ( SUBTEST_NUMBER "b" )
# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.cmp.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.src.json COPYONLY)

# Register test with CTest
add_test(NAME JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/JSONOverlay_t -m 8 -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

SET ( TEST_NUMBER "005" )
SET ( SUBTEST_NUMBER "a" )
# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.cmp.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.src.json COPYONLY)

# Register test with CTest
add_test(NAME JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/JSONOverlay_t -f -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

SET ( SUBTEST_NUMBER "b" )
# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.cmp.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.src.json COPYONLY)

# Register test with CTest
add_test(NAME JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/JSONOverlay_t -m 128 -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})


SET ( TEST_NUMBER "006" )
SET ( SUBTEST_NUMBER "a" )
# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.cmp.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.src.json COPYONLY)

# Register test with CTest
add_test(NAME JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/JSONOverlay_t -f -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

SET ( SUBTEST_NUMBER "b" )
# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.cmp.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.src.json COPYONLY)

# Register test with CTest
add_test(NAME JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/JSONOverlay_t -m 536870912 -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})


SET ( TEST_NUMBER "007" )
SET ( SUBTEST_NUMBER "a" )
# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.cmp.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.src.json COPYONLY)

# Register test with CTest
add_test(NAME JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/JSONOverlay_t -f -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

SET ( SUBTEST_NUMBER "b" )
# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.cmp.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.src.json COPYONLY)

# Register test with CTest
add_test(NAME JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/JSONOverlay_t -m 1073741824 -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

  SET ( TEST_NUMBER "008" )
SET ( SUBTEST_NUMBER "a" )
# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.cmp.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.src.json COPYONLY)

# Register test with CTest
add_test(NAME JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/JSONOverlay_t -f -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

SET ( SUBTEST_NUMBER "b" )
# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.cmp.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.src.json COPYONLY)

# Register test with CTest
add_test(NAME JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/JSONOverlay_t -m 16 -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})


SET ( TEST_NUMBER "009" )
SET ( SUBTEST_NUMBER "a" )
# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.cmp.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.src.json COPYONLY)

# Register test with CTest
add_test(NAME JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/JSONOverlay_t -f -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

SET ( SUBTEST_NUMBER "b" )
# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.cmp.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.src.json COPYONLY)

# Register test with CTest
add_test(NAME JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/JSONOverlay_t -m 32 -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

SET ( TEST_NUMBER "010" )
SET ( SUBTEST_NUMBER "a" )
# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.cmp.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.src.json COPYONLY)

# Register test with CTest
add_test(NAME JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/JSONOverlay_t -f -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

SET ( SUBTEST_NUMBER "b" )
# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.cmp.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.src.json COPYONLY)

# Register test with CTest
add_test(NAME JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/JSONOverlay_t -m 512 -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

SET ( TEST_NUMBER "011" )
SET ( SUBTEST_NUMBER "a" )
# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.cmp.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.src.json COPYONLY)

# Register test with CTest
add_test(NAME JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/JSONOverlay_t -f -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

SET ( SUBTEST_NUMBER "b" )
# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.cmp.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.src.json COPYONLY)

# Register test with CTest
add_test(NAME JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/JSONOverlay_t -m 1024 -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

SET ( TEST_NUMBER "012" )
SET ( SUBTEST_NUMBER "a" )
# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.cmp.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.src.json COPYONLY)

# Register test with CTest
add_test(NAME JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/JSONOverlay_t -f -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

SET ( SUBTEST_NUMBER "b" )
# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.cmp.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.src.json COPYONLY)

# Register test with CTest
add_test(NAME JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/JSONOverlay_t -m 2 -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

SET ( TEST_NUMBER "013" )
SET ( SUBTEST_NUMBER "a" )
# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.cmp.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.src.json COPYONLY)

# Register test with CTest
add_test(NAME JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/JSONOverlay_t -f -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

SET ( SUBTEST_NUMBER "b" )
# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.cmp.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json
               ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.src.json COPYONLY)

# Register test with CTest
add_test(NAME JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/JSONOverlay_t -m 1 -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
