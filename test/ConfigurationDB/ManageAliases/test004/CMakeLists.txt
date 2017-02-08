SET ( TEST_NUMBER "004" )
SET ( TEST_NAME "newconfig" )



configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/testtest0041.flt.json
  ${CMAKE_CURRENT_BINARY_DIR}/testtest0041.flt.${RAND_NAME}.json @ONLY )

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/testtest0042.flt.json
  ${CMAKE_CURRENT_BINARY_DIR}/testtest0042.flt.${RAND_NAME}.json @ONLY )
  

cet_test(conftool_mongo_${TEST_NAME}_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC conftool 
  DATAFILES 
    ${CMAKE_CURRENT_BINARY_DIR}/testtest0041.flt.${RAND_NAME}.json    
  TEST_ARGS  -o newconfig -c testtest0041.flt.${RAND_NAME}.json -j testtest0041.gui.${RAND_NAME}.json)

  
cet_test(conftool_filesystem_${TEST_NAME}_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC conftool 
  DATAFILES 
    ${CMAKE_CURRENT_BINARY_DIR}/testtest0042.flt.${RAND_NAME}.json 
  TEST_ARGS  -o newconfig -c testtest0042.flt.${RAND_NAME}.json -j testtest0042.gui.${RAND_NAME}.json)
