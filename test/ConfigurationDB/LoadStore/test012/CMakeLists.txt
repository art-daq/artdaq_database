SET ( TEST_NAME "LoadStore_store" )

SET ( TEST_NUMBER "012" )

SET ( TEST_DBPROVIDER "filesystem" )
string(RANDOM LENGTH 3 ALPHABET 0123456789 RAND_NAME)
configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test011.src.fcl ${CMAKE_CURRENT_BINARY_DIR}/test011.src.${RAND_NAME}.fcl @ONLY )
configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test011.cmp.fcl ${CMAKE_CURRENT_BINARY_DIR}/test011.cmp.${RAND_NAME}.fcl @ONLY )
configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test011.flt.json ${CMAKE_CURRENT_BINARY_DIR}/test011.flt.${RAND_NAME}.json @ONLY )
cet_test(${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_t HANDBUILT  TEST_EXEC LoadStore_t 
  DATAFILES 
    ${CMAKE_CURRENT_BINARY_DIR}/test011.src.${RAND_NAME}.fcl 
    ${CMAKE_CURRENT_BINARY_DIR}/test011.cmp.${RAND_NAME}.fcl
    ${CMAKE_CURRENT_BINARY_DIR}/test011.flt.${RAND_NAME}.json
  TEST_ARGS  --searchquery=test011.flt.${RAND_NAME}.json -s test011.src.${RAND_NAME}.fcl -r test011.cmp.${RAND_NAME}.fcl )

SET ( TEST_DBPROVIDER "mongo" )
string(RANDOM LENGTH 3 ALPHABET 0123456789 RAND_NAME)
configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test011.src.fcl ${CMAKE_CURRENT_BINARY_DIR}/test011.src.${RAND_NAME}.fcl @ONLY )
configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test011.cmp.fcl ${CMAKE_CURRENT_BINARY_DIR}/test011.cmp.${RAND_NAME}.fcl @ONLY )
configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test011.flt.json ${CMAKE_CURRENT_BINARY_DIR}/test011.flt.${RAND_NAME}.json @ONLY )
cet_test(${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_t HANDBUILT  TEST_EXEC LoadStore_t 
  DATAFILES 
    ${CMAKE_CURRENT_BINARY_DIR}/test011.src.${RAND_NAME}.fcl 
    ${CMAKE_CURRENT_BINARY_DIR}/test011.cmp.${RAND_NAME}.fcl 
    ${CMAKE_CURRENT_BINARY_DIR}/test011.flt.${RAND_NAME}.json
  TEST_ARGS  --searchquery=test011.flt.${RAND_NAME}.json -s test011.src.${RAND_NAME}.fcl -r test011.cmp.${RAND_NAME}.fcl )
