SET ( TEST_NUMBER "004" )

SET ( TEST_NAME "search2" )

cet_test(MongoXml_${TEST_NAME}_${TEST_NUMBER}_t HANDBUILT  TEST_EXEC MongoXml_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.xml 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.xml
  TEST_ARGS -s test${TEST_NUMBER}.src.xml -c test${TEST_NUMBER}.cmp.xml -t ${TEST_NAME})
