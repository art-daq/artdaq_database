SET ( TEST_NUMBER "103" )
SET ( TEST_NAME "insertChild" )


cet_test(JSONDocument_${TEST_NAME}_${TEST_NUMBER}_t HANDBUILT  TEST_EXEC JSONDocument_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.json 
    ${CMAKE_CURRENT_SOURCE_DIR}/begin-state.json 
    ${CMAKE_CURRENT_SOURCE_DIR}/end-state.json 
    ${CMAKE_CURRENT_SOURCE_DIR}/operation-delta.json 
    ${CMAKE_CURRENT_SOURCE_DIR}/returned-value.json 
  TEST_ARGS -c test${TEST_NUMBER}.json )
