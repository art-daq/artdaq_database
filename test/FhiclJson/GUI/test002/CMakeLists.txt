SET ( TEST_NUMBER "002" )
SET ( TEST_NAME "convert2fcl" )

cet_test(FhiclJsonGUI_${TEST_NAME}_${TEST_NUMBER}_t HANDBUILT  TEST_EXEC FhiclJsonGUI_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.fcl 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.json
  TEST_ARGS -s test${TEST_NUMBER}.json -c test${TEST_NUMBER}.fcl -t ${TEST_NAME})
