SET ( TEST_NUMBER "003" )

SET ( TEST_NAME "update" )

cet_test(FileSystemXml_${TEST_NAME}_${TEST_NUMBER}_t HANDBUILT  TEST_EXEC FileSystemXml_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.xml 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.xml
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.update.xml     
  TEST_ARGS -s test${TEST_NUMBER}.src.xml -c test${TEST_NUMBER}.cmp.xml -t ${TEST_NAME}  -o test${TEST_NUMBER}.update.xml)
