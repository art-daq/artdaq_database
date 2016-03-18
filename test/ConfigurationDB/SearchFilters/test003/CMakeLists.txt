SET ( TEST_NUMBER "003" )
SET ( TEST_NAME "findconfigs" )

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0031.flt.json 
  ${CMAKE_CURRENT_BINARY_DIR}/test0031.flt.json  @ONLY )

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0031.flt.json 
  ${CMAKE_CURRENT_BINARY_DIR}/test0032.flt.json  @ONLY )
  
cet_test(SearchFilters_mongo_${TEST_NAME}_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC conftool2 
  DATAFILES 
    ${CMAKE_CURRENT_BINARY_DIR}/test0031.flt.json 
  TEST_ARGS  -o findconfigs -c test0031.flt.json -j test0031.gui.json)

  
#cet_test(SearchFilters_filesystem_${TEST_NAME}_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC conftool2 
#  DATAFILES 
#    ${CMAKE_CURRENT_BINARY_DIR}/test0032.flt.json 
#  TEST_ARGS  -o findconfigs -c test0032.flt.json -j test0032.gui.json)

