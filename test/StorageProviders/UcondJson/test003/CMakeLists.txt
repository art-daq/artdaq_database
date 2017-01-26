SET ( TEST_NUMBER "003" )

SET ( TEST_NAME "update" )

cet_test(UcondJson_${TEST_NAME}_${TEST_NUMBER}_t HANDBUILT  TEST_EXEC UcondJson_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.update.json     
  TEST_ARGS -s test${TEST_NUMBER}.src.json -c test${TEST_NUMBER}.cmp.json -t ${TEST_NAME}  -o test${TEST_NUMBER}.update.json)
