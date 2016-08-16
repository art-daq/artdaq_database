SET ( TEST_NAME "LoadStore_store" )

SET ( TEST_NUMBER "001" )

SET ( TEST_DBPROVIDER "filesystem" )
string(RANDOM LENGTH 3 ALPHABET 0123456789 RAND_NAME)
configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test001.src.json ${CMAKE_CURRENT_BINARY_DIR}/test001.src.${RAND_NAME}.json @ONLY )
configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test001.cmp.json ${CMAKE_CURRENT_BINARY_DIR}/test001.cmp.${RAND_NAME}.json @ONLY )
cet_test(${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_t HANDBUILT  TEST_EXEC LoadStore_t 
  DATAFILES 
    ${CMAKE_CURRENT_BINARY_DIR}/test001.src.${RAND_NAME}.json 
    ${CMAKE_CURRENT_BINARY_DIR}/test001.cmp.${RAND_NAME}.json 
  TEST_ARGS -o store -d ${TEST_DBPROVIDER} -c ${TEST_COLLECTION_NAME} -f json -v ${TEST_VERSION_NAME}  -s test001.src.${RAND_NAME}.json -r test001.cmp.${RAND_NAME}.json )

SET ( TEST_DBPROVIDER "mongo" )
string(RANDOM LENGTH 3 ALPHABET 0123456789 RAND_NAME)
configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test001.src.json ${CMAKE_CURRENT_BINARY_DIR}/test001.src.${RAND_NAME}.json @ONLY )
configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test001.cmp.json ${CMAKE_CURRENT_BINARY_DIR}/test001.cmp.${RAND_NAME}.json @ONLY )
cet_test(${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_t HANDBUILT  TEST_EXEC LoadStore_t 
  DATAFILES 
    ${CMAKE_CURRENT_BINARY_DIR}/test001.src.${RAND_NAME}.json 
    ${CMAKE_CURRENT_BINARY_DIR}/test001.cmp.${RAND_NAME}.json 
  TEST_ARGS -o store -d ${TEST_DBPROVIDER} -c ${TEST_COLLECTION_NAME} -f json -v ${TEST_VERSION_NAME} -s test001.src.${RAND_NAME}.json -r test001.cmp.${RAND_NAME}.json )
