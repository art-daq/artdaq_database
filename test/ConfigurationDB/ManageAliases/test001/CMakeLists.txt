SET ( TEST_NUMBER "001" )
SET ( TEST_NAME "findversions" )

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0011.cmp.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0011.cmp.${RAND_NAME}.json @ONLY )

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0012.cmp.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0012.cmp.${RAND_NAME}.json @ONLY )

  
cet_test(GlobalConfigBuilder_mongo_${TEST_NAME}_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC ConfigAliasBuilder_t 
  DATAFILES 
    ${CMAKE_CURRENT_BINARY_DIR}/test0011.cmp.${RAND_NAME}.json 
  TEST_ARGS  -d mongo -o findversions -e ${TEST_ENTITY_NAME} -c artdaq_Aggregator -s test0011.cmp.${RAND_NAME}.json)
  
cet_test(GlobalConfigBuilder_filesystem_${TEST_NAME}_${TEST_NUMBER}_02_t HANDBUILT  TEST_EXEC ConfigAliasBuilder_t 
  DATAFILES 
    ${CMAKE_CURRENT_BINARY_DIR}/test0012.cmp.${RAND_NAME}.json 
  TEST_ARGS  -d filesystem -o findversions -e ${TEST_ENTITY_NAME} -c artdaq_Aggregator -s test0012.cmp.${RAND_NAME}.json)


configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0011.flt.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0011.flt.${RAND_NAME}.json @ONLY )

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0012.flt.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0012.flt.${RAND_NAME}.json @ONLY )
  

cet_test(conftool2_mongo_${TEST_NAME}_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC conftool2 
  DATAFILES 
    ${CMAKE_CURRENT_BINARY_DIR}/test0011.flt.${RAND_NAME}.json    
  TEST_ARGS  -o findversions -c test0011.flt.${RAND_NAME}.json -j test0011.gui.${RAND_NAME}.json)

  
cet_test(conftool2_filesystem_${TEST_NAME}_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC conftool2 
  DATAFILES 
    ${CMAKE_CURRENT_BINARY_DIR}/test0012.flt.${RAND_NAME}.json 
  TEST_ARGS  -o findversions -c test0012.flt.${RAND_NAME}.json -j test0012.gui.${RAND_NAME}.json)
