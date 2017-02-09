SET ( TEST_NUMBER "004" )
SET ( TEST_NAME "store" )


cet_test(conftool_filesystem_${TEST_NAME}_${TEST_NUMBER}_12_t HANDBUILT  TEST_EXEC conftool 
  DATAFILES 
      ${CMAKE_CURRENT_SOURCE_DIR}/test004.flt.filesystem.json 
      ${CMAKE_CURRENT_SOURCE_DIR}/test004.gui.filesystem.json 
  TEST_ARGS -o store -c test004.flt.filesystem.json -j test004.gui.filesystem.json )
