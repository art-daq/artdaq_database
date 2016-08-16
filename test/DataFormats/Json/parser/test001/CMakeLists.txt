SET ( TEST_NAME "Readjson" )

SET ( TEST_NUMBER "001" )
cet_test(${TEST_NAME}_${TEST_NUMBER}_t HANDBUILT  TEST_EXEC readjson_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.json 
  TEST_ARGS -c test${TEST_NUMBER}.json )

SET ( TEST_NUMBER "002" )
cet_test(${TEST_NAME}_${TEST_NUMBER}_t HANDBUILT  TEST_EXEC readjson_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.json 
  TEST_ARGS -c test${TEST_NUMBER}.json )

SET ( TEST_NUMBER "003" )
cet_test(${TEST_NAME}_${TEST_NUMBER}_t HANDBUILT  TEST_EXEC readjson_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.json 
  TEST_ARGS -c test${TEST_NUMBER}.json )

SET ( TEST_NUMBER "004" )
cet_test(${TEST_NAME}_${TEST_NUMBER}_t HANDBUILT  TEST_EXEC readjson_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.json 
  TEST_ARGS -c test${TEST_NUMBER}.json )

SET ( TEST_NUMBER "005" )
cet_test(${TEST_NAME}_${TEST_NUMBER}_t HANDBUILT  TEST_EXEC readjson_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.json 
  TEST_ARGS -c test${TEST_NUMBER}.json )


SET ( TEST_NUMBER "006" )
cet_test(${TEST_NAME}_${TEST_NUMBER}_t HANDBUILT  TEST_EXEC readjson_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.json 
  TEST_ARGS -c test${TEST_NUMBER}.json )
    