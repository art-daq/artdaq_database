SET ( TEST_NUMBER "003" )
SET ( TEST_NAME "buildDocument" )


cet_test(JSONDocumentBuilder_${TEST_NAME}_${TEST_NUMBER}_t HANDBUILT  TEST_EXEC JSONDocumentBuilder_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.json 
    ${CMAKE_CURRENT_SOURCE_DIR}/begin-state.json 
    ${CMAKE_CURRENT_SOURCE_DIR}/end-state.json 
  TEST_ARGS -c test${TEST_NUMBER}.json )
