SET ( TEST_NUMBER "002" )
SET ( TEST_NAME "store" )


cet_test(ArtdaqDemo_mongo_${TEST_NAME}_${TEST_NUMBER}_t HANDBUILT  TEST_EXEC ArtdaqDemo_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test002.src.json 
  TEST_ARGS -d mongo -o store -t test_${TEST_NAME}_${TEST_NUMBER} -c test002.src.json -f json -v version -g config )

  
cet_test(ArtdaqDemo_filesystem_${TEST_NAME}_${TEST_NUMBER}_t HANDBUILT  TEST_EXEC ArtdaqDemo_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test002.src.json 
  TEST_ARGS -d filesystem -o store -t test_${TEST_NAME}_${TEST_NUMBER} -c test002.src.json -f json -v version -g config )
  