SET ( TEST_NAME "RoundConvert_DBJson2Fhicl" )

SET ( TEST_NUMBER "004" )
cet_test(${TEST_NAME}_${TEST_NUMBER}_t HANDBUILT  TEST_EXEC ConvertFhicl_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.json
  TEST_ARGS -s test${TEST_NUMBER}.json -c test${TEST_NUMBER}.json -t ${TEST_NAME})
