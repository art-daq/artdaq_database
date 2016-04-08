SET ( TEST_NUMBER "003" )

SET ( TEST_NAME "update" )

cet_test(FileSystemFhicl_${TEST_NAME}_${TEST_NUMBER}_t HANDBUILT  TEST_EXEC FileSystemFhicl_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.fcl 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.fcl
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.update.fcl     
  TEST_ARGS -s test${TEST_NUMBER}.src.fcl -c test${TEST_NUMBER}.cmp.fcl -t ${TEST_NAME}  -o test${TEST_NUMBER}.update.fcl)
