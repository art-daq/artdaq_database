SET ( TEST_NUMBER "001" )
SET ( TEST_NAME "xml2json" )

cet_test(XMLConvert_${TEST_NAME}_${TEST_NUMBER}_t HANDBUILT  TEST_EXEC XMLConvert_t
  DATAFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.xml 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.json
  TEST_ARGS -s test${TEST_NUMBER}.xml -c test${TEST_NUMBER}.json -t ${TEST_NAME})


