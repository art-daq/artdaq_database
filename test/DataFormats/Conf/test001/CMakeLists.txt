SET ( TEST_NUMBER "001" )
SET ( TEST_NAME "conf2json" )

cet_test(CONFConvert_${TEST_NAME}_${TEST_NUMBER}_t HANDBUILT  TEST_EXEC CONFConvert_t
  DATAFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.conf 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.json
  TEST_ARGS -s test${TEST_NUMBER}.conf -c test${TEST_NUMBER}.json -t ${TEST_NAME})


