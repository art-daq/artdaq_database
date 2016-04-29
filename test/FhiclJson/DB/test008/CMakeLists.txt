SET ( TEST_NUMBER "008" )

SET ( TEST_NAME "convert2fcl" )

cet_test(FhiclJsonDB_${TEST_NAME}_${TEST_NUMBER}_t HANDBUILT  TEST_EXEC FhiclJsonDB_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.fcl 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.json
  TEST_ARGS -s test${TEST_NUMBER}.json -c test${TEST_NUMBER}.fcl -t ${TEST_NAME})
