SET ( TEST_NUMBER "002" )
SET ( TEST_NAME "json2xml" )

cet_test(XMLConvert_${TEST_NAME}_${TEST_NUMBER}_t HANDBUILT  TEST_EXEC XMLConvert_t
  DATAFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.xml 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.json
  TEST_ARGS -s test${TEST_NUMBER}.json -c test${TEST_NUMBER}.xml -t ${TEST_NAME})


