SET ( TEST_NUMBER "002" )
SET ( TEST_NAME "buildfilter" )

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test002.cmp.json
  ${CMAKE_CURRENT_BINARY_DIR}/test002.cmp.json @ONLY )

cet_test(SearchFilters_mongo_${TEST_NAME}_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC SearchFilters_t 
  DATAFILES 
     ${CMAKE_CURRENT_BINARY_DIR}/test002.cmp.json 
  TEST_ARGS -d mongo -o buildfilter  -g testConfigA${CONFIG_NUMBER} -c test002.cmp.json )

  
#cet_test(SearchFilters_filesystem_${TEST_NAME}_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC SearchFilters_t 
#  DATAFILES 
#    ${CMAKE_CURRENT_BINARY_DIR}/test002.cmp.json 
#  TEST_ARGS -d filesystem  -o buildfilter -g testConfigA${CONFIG_NUMBER}  -c test002.cmp.json)
