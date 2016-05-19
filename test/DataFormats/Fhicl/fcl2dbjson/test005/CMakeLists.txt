SET ( TEST_NAME "Convert_Fhicl2DBJson" )

SET ( TEST_NUMBER "005" )
cet_test(${TEST_NAME}_${TEST_NUMBER}_t HANDBUILT  TEST_EXEC ConvertFhicl_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.fcl 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.json
  TEST_ARGS -s test${TEST_NUMBER}.fcl -c test${TEST_NUMBER}.json -t ${TEST_NAME})
