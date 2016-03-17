SET ( TEST_NUMBER "001" )
SET ( TEST_NAME "findconfigs" )


cet_test(SearchFilters_mongo_${TEST_NAME}_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC SearchFilters_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test001.cmp.json 
  TEST_ARGS -d mongo -o findconfigs  -c test001.cmp.json )

  
cet_test(SearchFilters_filesystem_${TEST_NAME}_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC SearchFilters_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test001.cmp.json 
  TEST_ARGS -d filesystem  -o findconfigs  -c test001.cmp.json)

