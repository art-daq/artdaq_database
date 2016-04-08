SET ( TEST_NUMBER "001" )
SET ( TEST_NAME "load" )

cet_test(ConfToolTest_${TEST_NAME}_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC ConfToolTest_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.flt.json
  TEST_ARGS -s test${TEST_NUMBER}.src.json -c test${TEST_NUMBER}.src.fcl -f test${TEST_NUMBER}.flt.json -o store)
  
