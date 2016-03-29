SET ( TEST_NUMBER "003" )
SET ( TEST_NAME "findentities" )


configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0031.cmp.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0031.cmp.${RAND_NAME}.json @ONLY )

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0032.cmp.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0032.cmp.${RAND_NAME}.json @ONLY )

  
cet_test(GlobalConfigBuilder_mongo_${TEST_NAME}_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC GlobalConfigBuilder_t 
  DATAFILES 
    ${CMAKE_CURRENT_BINARY_DIR}/test0031.cmp.${RAND_NAME}.json 
  TEST_ARGS  -d mongo -o findentities -t artdaq_Aggregator -v version004 -c test0031.cmp.${RAND_NAME}.json)

cet_test(GlobalConfigBuilder_mongo_${TEST_NAME}_${TEST_NUMBER}_02_t HANDBUILT  TEST_EXEC GlobalConfigBuilder_t 
  DATAFILES 
    ${CMAKE_CURRENT_BINARY_DIR}/test0031.cmp.${RAND_NAME}.json 
  TEST_ARGS  -d mongo -o findentities -t artdaq_Aggregator -c test0031.cmp.${RAND_NAME}.json)
  

cet_test(GlobalConfigBuilder_filesystem_${TEST_NAME}_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC GlobalConfigBuilder_t 
  DATAFILES 
    ${CMAKE_CURRENT_BINARY_DIR}/test0032.cmp.${RAND_NAME}.json 
  TEST_ARGS  -d filesystem -o findentities -t artdaq_Aggregator -v version004 -c test0032.cmp.${RAND_NAME}.json)
  


cet_test(GlobalConfigBuilder_filesystem_${TEST_NAME}_${TEST_NUMBER}_02_t HANDBUILT  TEST_EXEC GlobalConfigBuilder_t 
  DATAFILES 
    ${CMAKE_CURRENT_BINARY_DIR}/test0032.cmp.${RAND_NAME}.json 
  TEST_ARGS  -d filesystem -o findentities -t artdaq_Aggregator -c test0032.cmp.${RAND_NAME}.json)
  

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0031.flt.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0031.flt.${RAND_NAME}.json @ONLY )

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0032.flt.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0032.flt.${RAND_NAME}.json @ONLY )
  

cet_test(conftool2_mongo_${TEST_NAME}_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC conftool2 
  DATAFILES 
    ${CMAKE_CURRENT_BINARY_DIR}/test0031.flt.${RAND_NAME}.json 
  TEST_ARGS  -o findentities -c test0031.flt.${RAND_NAME}.json -j test0031.gui.${RAND_NAME}.json)

  
cet_test(conftool2_filesystem_${TEST_NAME}_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC conftool2 
  DATAFILES 
   ${CMAKE_CURRENT_BINARY_DIR}/test0032.flt.${RAND_NAME}.json 
  TEST_ARGS  -o findentities -c test0032.flt.${RAND_NAME}.json -j test0032.gui.${RAND_NAME}.json)  