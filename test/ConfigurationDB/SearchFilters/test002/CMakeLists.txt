SET ( TEST_NUMBER "001" )
SET ( TEST_NAME "buildfilter" )


cet_test(SearchFilters_mongo_${TEST_NAME}_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC SearchFilters_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test001.flt.json 
  TEST_ARGS -d mongo -o buildfilter  -c test001.flt.json )

  
cet_test(SearchFilters_filesystem_${TEST_NAME}_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC SearchFilters_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test001.flt.json 
  TEST_ARGS -d filesystem  -o buildfilter  -c test001.flt.json)

