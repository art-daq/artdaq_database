SET ( TEST_NUMBER "005" )
SET ( TEST_NAME "store" )


cet_test(conftool2_filesystem_${TEST_NAME}_${TEST_NUMBER}_12_t HANDBUILT  TEST_EXEC conftool2 
  DATAFILES 
      ${CMAKE_CURRENT_SOURCE_DIR}/test005.flt.filesystem.json 
      ${CMAKE_CURRENT_SOURCE_DIR}/test005.gui.filesystem.json 
  TEST_ARGS -o store -c test005.flt.filesystem.json -j test005.gui.filesystem.json )
