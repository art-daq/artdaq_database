SET ( TEST_NAME "ReadWrite_store" )

SET ( TEST_NUMBER "021" )

SET ( TEST_DBPROVIDER "filesystem" )
cet_test_env("ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}")
string(RANDOM LENGTH 3 ALPHABET 0123456789 RAND_NAME)
configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test011.src.json ${CMAKE_CURRENT_BINARY_DIR}/test011.src.${RAND_NAME}.json @ONLY )
configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test011.cmp.json ${CMAKE_CURRENT_BINARY_DIR}/test011.cmp.${RAND_NAME}.json @ONLY )
configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test011.flt.json ${CMAKE_CURRENT_BINARY_DIR}/test011.flt.${RAND_NAME}.json @ONLY )
cet_test(${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_t HANDBUILT  TEST_EXEC ReadWrite_t 
  DATAFILES 
    ${CMAKE_CURRENT_BINARY_DIR}/test011.src.${RAND_NAME}.json 
    ${CMAKE_CURRENT_BINARY_DIR}/test011.cmp.${RAND_NAME}.json
    ${CMAKE_CURRENT_BINARY_DIR}/test011.flt.${RAND_NAME}.json
  TEST_ARGS  --searchquery=test011.flt.${RAND_NAME}.json -s test011.src.${RAND_NAME}.json -x test011.cmp.${RAND_NAME}.json )

