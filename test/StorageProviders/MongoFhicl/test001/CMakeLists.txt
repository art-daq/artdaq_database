SET ( TEST_NUMBER "001" )

SET ( TEST_NAME "insert" )

cet_test(MongoFhicl_${TEST_NAME}_${TEST_NUMBER}_t HANDBUILT  TEST_EXEC MongoFhicl_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.fcl 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.fcl
  TEST_ARGS -s test${TEST_NUMBER}.src.fcl -c test${TEST_NUMBER}.cmp.fcl -t ${TEST_NAME})

