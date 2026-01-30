SET ( TEST_NUMBER "001" )
SET ( TEST_NAME "jsonconvert" )

# Register test with CTest
add_test(NAME JSONConvert_01_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/JSONConvert_t.cc
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
