SET ( TEST_NUMBER "002" )
SET ( TEST_NAME "json2conf" )

cet_test(CONFConvert_${TEST_NAME}_${TEST_NUMBER}_t HANDBUILT  TEST_EXEC CONFConvert_t
  DATAFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.conf 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.json
  TEST_ARGS -s test${TEST_NUMBER}.json -c test${TEST_NUMBER}.conf -t ${TEST_NAME})


