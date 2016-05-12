SET ( TEST_NAME "LoadStore_store" )

SET ( TEST_NUMBER "002" )

SET ( TEST_DBPROVIDER "filesystem" )
configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test001.src.fcl ${CMAKE_CURRENT_BINARY_DIR}/test001.src.${RAND_NAME}.fcl @ONLY )
configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test001.cmp.fcl ${CMAKE_CURRENT_BINARY_DIR}/test001.cmp.${RAND_NAME}.fcl @ONLY )
cet_test(${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_t HANDBUILT  TEST_EXEC LoadStore_t 
  DATAFILES 
    ${CMAKE_CURRENT_BINARY_DIR}/test001.src.${RAND_NAME}.fcl 
    ${CMAKE_CURRENT_BINARY_DIR}/test001.cmp.${RAND_NAME}.fcl 
  TEST_ARGS -o store -d ${TEST_DBPROVIDER} -c ${TEST_COLLECTION_NAME} -f fhicl -v version${VERSION_NUMBER} -s test001.src.${RAND_NAME}.fcl -r test001.cmp.${RAND_NAME}.fcl )



SET ( TEST_DBPROVIDER "mongo" )
configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test001.src.fcl ${CMAKE_CURRENT_BINARY_DIR}/test001.src.${RAND_NAME}.fcl @ONLY )
configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test001.cmp.fcl ${CMAKE_CURRENT_BINARY_DIR}/test001.cmp.${RAND_NAME}.fcl @ONLY )
cet_test(${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_t HANDBUILT  TEST_EXEC LoadStore_t 
  DATAFILES 
    ${CMAKE_CURRENT_BINARY_DIR}/test001.src.${RAND_NAME}.fcl 
    ${CMAKE_CURRENT_BINARY_DIR}/test001.cmp.${RAND_NAME}.fcl 
  TEST_ARGS -o store -d ${TEST_DBPROVIDER} -c ${TEST_COLLECTION_NAME} -f fhicl -v version${VERSION_NUMBER} -s test001.src.${RAND_NAME}.fcl -r test001.cmp.${RAND_NAME}.fcl )
