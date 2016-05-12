SET ( TEST_NUMBER "004" )
SET ( TEST_NAME "buildfilter" )

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0041.flt.json 
  ${CMAKE_CURRENT_BINARY_DIR}/test0041.flt.json  @ONLY )

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0041.flt.json 
  ${CMAKE_CURRENT_BINARY_DIR}/test0042.flt.json  @ONLY )
  
cet_test(conftool2_mongo_${TEST_NAME}_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC conftool2 
  DATAFILES 
    ${CMAKE_CURRENT_BINARY_DIR}/test0041.flt.json 
  TEST_ARGS  -o buildfilter -c test0041.flt.json -j test0041.gui.json)

  
cet_test(conftool2_filesystem_${TEST_NAME}_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC conftool2 
  DATAFILES 
    ${CMAKE_CURRENT_BINARY_DIR}/test0042.flt.json 
  TEST_ARGS  -o buildfilter -c test0042.flt.json -j test0042.gui.json)

