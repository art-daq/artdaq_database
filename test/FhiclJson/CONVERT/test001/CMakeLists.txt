SET ( TEST_NUMBER "001" )

SET ( TEST_NAME "convert2gui" )

cet_test(ConvertJson_${TEST_NAME}_${TEST_NUMBER}_t HANDBUILT  TEST_EXEC ConvertJson_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
  TEST_ARGS -s test${TEST_NUMBER}.src.json -c test${TEST_NUMBER}.cmp.json -t ${TEST_NAME})
