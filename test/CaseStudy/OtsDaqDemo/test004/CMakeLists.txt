SET ( TEST_NUMBER "004" )
SET ( TEST_NAME "store" )


# Copy test data files to build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test004.flt.filesystem.json
               ${CMAKE_CURRENT_BINARY_DIR}/test004.flt.filesystem.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test004.gui.filesystem.json
               ${CMAKE_CURRENT_BINARY_DIR}/test004.gui.filesystem.json COPYONLY)

# Register test with CTest
add_test(NAME conftool_filesystem_${TEST_NAME}_${TEST_NUMBER}_12_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/conftool -o store -c test004.flt.filesystem.json -j test004.gui.filesystem.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
