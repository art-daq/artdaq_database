SET ( TEST_NAME "RoundConvert_Fhicl2DBJson" )

SET ( TEST_NUMBER "001" )

cet_test(${TEST_NAME}_${TEST_NUMBER}_t HANDBUILT  TEST_EXEC ConvertFhicl_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.fcl 
  TEST_ARGS -s test${TEST_NUMBER}.fcl -c test${TEST_NUMBER}.fcl -t ${TEST_NAME})

  
SET ( TEST_NUMBER "002" )

cet_test(${TEST_NAME}_${TEST_NUMBER}_t HANDBUILT  TEST_EXEC ConvertFhicl_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.fcl 
  TEST_ARGS -s test${TEST_NUMBER}.fcl -c test${TEST_NUMBER}.fcl -t ${TEST_NAME})


SET ( TEST_NUMBER "003" )

cet_test(${TEST_NAME}_${TEST_NUMBER}_t HANDBUILT  TEST_EXEC ConvertFhicl_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.fcl 
  TEST_ARGS -s test${TEST_NUMBER}.fcl -c test${TEST_NUMBER}.fcl -t ${TEST_NAME})

SET ( TEST_NUMBER "004" )

cet_test(${TEST_NAME}_${TEST_NUMBER}_t HANDBUILT  TEST_EXEC ConvertFhicl_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.fcl 
  TEST_ARGS -s test${TEST_NUMBER}.fcl -c test${TEST_NUMBER}.fcl -t ${TEST_NAME})
  