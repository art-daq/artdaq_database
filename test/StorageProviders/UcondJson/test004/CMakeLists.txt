SET ( TEST_NUMBER "004" )

SET ( TEST_NAME "search2" )

cet_test(UcondJson_${TEST_NAME}_${TEST_NUMBER}_t HANDBUILT  TEST_EXEC UcondJson_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
  TEST_ARGS -s test${TEST_NUMBER}.src.json -c test${TEST_NUMBER}.cmp.json -t ${TEST_NAME})
