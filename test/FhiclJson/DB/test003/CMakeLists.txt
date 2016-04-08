SET ( TEST_NUMBER "003" )

SET ( TEST_NAME "roundconvertfcl" )

cet_test(FhiclJsonDB_${TEST_NAME}_${TEST_NUMBER}_t HANDBUILT  TEST_EXEC FhiclJsonDB_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.fcl 
  TEST_ARGS -s test${TEST_NUMBER}.fcl -c test${TEST_NUMBER}.fcl -t ${TEST_NAME})
