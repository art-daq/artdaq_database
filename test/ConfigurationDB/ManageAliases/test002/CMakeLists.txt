SET ( TEST_NUMBER "002" )
SET ( TEST_NAME "addconfig" )


configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0021.cmp.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0021.cmp.${RAND_NAME}.json @ONLY )

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0022.cmp.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0022.cmp.${RAND_NAME}.json @ONLY )

  
cet_test(GlobalConfigBuilder_mongo_${TEST_NAME}_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC GlobalConfigBuilder_t 
  DATAFILES 
    ${CMAKE_CURRENT_BINARY_DIR}/test0021.cmp.${RAND_NAME}.json 
  TEST_ARGS  -p mongo -o addconfig -e ${TEST_ENTITY_NAME} -t artdaq_Aggregator -g configA003 -v version004 -c test0021.cmp.${RAND_NAME}.json)

cet_test(GlobalConfigBuilder_filesystem_${TEST_NAME}_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC GlobalConfigBuilder_t 
  DATAFILES 
    ${CMAKE_CURRENT_BINARY_DIR}/test0022.cmp.${RAND_NAME}.json 
  TEST_ARGS  -p filesystem -o addconfig -e ${TEST_ENTITY_NAME} -t artdaq_Aggregator -g configA003 -v version004 -c test0022.cmp.${RAND_NAME}.json)


