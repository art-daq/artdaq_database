SET ( TEST_NUMBER "007" )

SET ( TEST_NAME "convert2json" )

#cet_test(readfhicl_${TEST_NAME}_${TEST_NUMBER}_t HANDBUILT  TEST_EXEC readfhicl 
#  DATAFILES 
#    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.fcl 
# TEST_ARGS -c test${TEST_NUMBER}.fcl )

  
cet_test(${TEST_NAME}_${TEST_NUMBER}_t HANDBUILT  TEST_EXEC ConvertFhicl_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.fcl 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.json
  TEST_ARGS -s test${TEST_NUMBER}.fcl -c test${TEST_NUMBER}.json -t ${TEST_NAME})
