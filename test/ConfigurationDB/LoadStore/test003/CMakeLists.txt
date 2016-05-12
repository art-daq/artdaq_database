SET ( TEST_NAME "LoadStore_load" )

SET ( TEST_NUMBER "001" )

SET ( TEST_DBPROVIDER "filesystem" )
configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test001.cmp.json ${CMAKE_CURRENT_BINARY_DIR}/test001.cmp.${RAND_NAME}.json @ONLY )
cet_test(${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_t HANDBUILT  TEST_EXEC LoadStore_t 
  DATAFILES 
    ${CMAKE_CURRENT_BINARY_DIR}/test001.cmp.${RAND_NAME}.json 
  TEST_ARGS -o load -d ${TEST_DBPROVIDER} -c ${TEST_COLLECTION_NAME} -f json -v version${VERSION_NUMBER} -r test001.cmp.${RAND_NAME}.json )


SET ( TEST_DBPROVIDER "mongo" )
configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test001.cmp.json ${CMAKE_CURRENT_BINARY_DIR}/test001.cmp.${RAND_NAME}.json @ONLY )
cet_test(${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_t HANDBUILT  TEST_EXEC LoadStore_t 
  DATAFILES 
    ${CMAKE_CURRENT_BINARY_DIR}/test001.cmp.${RAND_NAME}.json 
  TEST_ARGS -o load -d ${TEST_DBPROVIDER} -c ${TEST_COLLECTION_NAME} -f json -v version${VERSION_NUMBER} -r test001.cmp.${RAND_NAME}.json )

