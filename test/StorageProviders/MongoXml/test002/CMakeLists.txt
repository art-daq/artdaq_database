SET ( TEST_NUMBER "002" )

SET ( TEST_NAME "search1" )


string(RANDOM LENGTH 3 ALPHABET 0123456789 RAND_NAME)
configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.xml ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.src.${RAND_NAME}.xml @ONLY )
configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.xml ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.cmp.${RAND_NAME}.xml @ONLY )
configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.opts.json ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.opts.${RAND_NAME}.json @ONLY )
cet_test(MongoXml_${TEST_NAME}_${TEST_NUMBER}_t HANDBUILT  TEST_EXEC MongoXml_t 
  DATAFILES 
    ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.src.${RAND_NAME}.xml
    ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.cmp.${RAND_NAME}.xml
    ${CMAKE_CURRENT_BINARY_DIR}/test${TEST_NUMBER}.opts.${RAND_NAME}.json
  TEST_ARGS -s test${TEST_NUMBER}.src.${RAND_NAME}.xml -c test${TEST_NUMBER}.cmp.${RAND_NAME}.xml -t ${TEST_NAME} -o test${TEST_NUMBER}.opts.${RAND_NAME}.json)

