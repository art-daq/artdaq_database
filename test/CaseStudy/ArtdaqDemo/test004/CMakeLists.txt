SET ( TEST_NUMBER "004" )
SET ( TEST_NAME "load" )

cet_test(ArtdaqDemo_mongo_${TEST_NAME}_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC ArtdaqDemo_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test004.src.json 
  TEST_ARGS -d mongo -o store -t test_${TEST_NAME}_${TEST_NUMBER} -c test004.src.json -f json -v version -g config-${TEST_NUMBER} )

  
cet_test(ArtdaqDemo_filesystem_${TEST_NAME}_${TEST_NUMBER}_02_t HANDBUILT  TEST_EXEC ArtdaqDemo_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test004.src.json 
  TEST_ARGS -d filesystem -o store -t test_${TEST_NAME}_${TEST_NUMBER} -c test004.src.json -f json -v version -g config-${TEST_NUMBER} )


cet_test(ArtdaqDemo_mongo_${TEST_NAME}_${TEST_NUMBER}_03_t HANDBUILT  TEST_EXEC ArtdaqDemo_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test004.cmp.json 
  TEST_ARGS -d mongo -o load -t test_${TEST_NAME}_${TEST_NUMBER} -c test004.out.json -f json -g config-${TEST_NUMBER} )

  
cet_test(ArtdaqDemo_filesystem_${TEST_NAME}_${TEST_NUMBER}_04_t HANDBUILT  TEST_EXEC ArtdaqDemo_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test004.cmp.json 
  TEST_ARGS -d filesystem -o load -t test_${TEST_NAME}_${TEST_NUMBER} -c test004.out.json -f json -g config-${TEST_NUMBER} )
    