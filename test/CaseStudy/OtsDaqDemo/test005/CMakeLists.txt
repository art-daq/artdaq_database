SET ( TEST_NUMBER "005" )
SET ( TEST_NAME "store" )


# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test005.flt.filesystem.json
               ${CMAKE_CURRENT_BINARY_DIR}/test005.flt.filesystem.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test005.gui.filesystem.json
               ${CMAKE_CURRENT_BINARY_DIR}/test005.gui.filesystem.json COPYONLY)

# Register test with CTest
add_test(NAME conftool_filesystem_${TEST_NAME}_${TEST_NUMBER}_12_t
         COMMAND ${CMAKE_BINARY_DIR}/bin/conftool -o store -searchquery=test005.flt.filesystem.json -x test005.gui.filesystem.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
