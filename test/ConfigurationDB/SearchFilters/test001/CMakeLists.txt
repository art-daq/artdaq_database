SET ( TEST_NUMBER "001" )
SET ( TEST_NAME "findconfigs" )

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test001.cmp.json
  ${CMAKE_CURRENT_BINARY_DIR}/test001.cmp.${RAND_NAME}.json @ONLY )

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test002.cmp.json
  ${CMAKE_CURRENT_BINARY_DIR}/test002.cmp.${RAND_NAME}.json @ONLY )

cet_test(SearchFilters_mongo_${TEST_NAME}_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC SearchFilters_t 
  DATAFILES 
    ${CMAKE_CURRENT_BINARY_DIR}/test001.cmp.${RAND_NAME}.json 
  TEST_ARGS -d mongo -o findconfigs  -c test001.cmp.${RAND_NAME}.json  -g testConfigA${CONFIG_NUMBER} )


cet_test(SearchFilters_filesystem_${TEST_NAME}_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC SearchFilters_t 
  DATAFILES 
    ${CMAKE_CURRENT_BINARY_DIR}/test002.cmp.${RAND_NAME}.json 
  TEST_ARGS -d filesystem -o findconfigs  -c test002.cmp.${RAND_NAME}.json )
#  -g testConfigA${CONFIG_NUMBER} )

