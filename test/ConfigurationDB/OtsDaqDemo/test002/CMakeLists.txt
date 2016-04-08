SET ( TEST_NUMBER "002" )
SET ( TEST_NAME "load" )


cet_test(OtsDaqDemo_mongo_${TEST_NAME}_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC OtsDaqDemo_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test001.src.json 
  TEST_ARGS -d mongo -o store -t test_OtsDaqDemo${TEST_NAME}_${TEST_NUMBER} -c test001.src.json -f json -v version -g config1 )

  
cet_test(OtsDaqDemo_filesystem_${TEST_NAME}_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC OtsDaqDemo_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test001.src.json 
  TEST_ARGS -d filesystem -o store -t test_OtsDaqDemo${TEST_NAME}_${TEST_NUMBER} -c test001.src.json -f json -v version -g config1 )

cet_test(OtsDaqDemo_mongo_${TEST_NAME}_${TEST_NUMBER}_02_t HANDBUILT  TEST_EXEC OtsDaqDemo_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test002.src.json 
  TEST_ARGS -d mongo -o store -t test_OtsDaqDemo${TEST_NAME}_${TEST_NUMBER} -c test002.src.json -f json -v version -g config2 )

  
cet_test(OtsDaqDemo_filesystem_${TEST_NAME}_${TEST_NUMBER}_02_t HANDBUILT  TEST_EXEC OtsDaqDemo_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test002.src.json 
  TEST_ARGS -d filesystem -o store -t test_OtsDaqDemo${TEST_NAME}_${TEST_NUMBER} -c test002.src.json -f json -v version -g config2 )

  
cet_test(OtsDaqDemo_mongo_${TEST_NAME}_${TEST_NUMBER}_11_t HANDBUILT  TEST_EXEC OtsDaqDemo_t 
  DATAFILES 
  TEST_ARGS -d mongo -o load -t test_OtsDaqDemo${TEST_NAME}_${TEST_NUMBER} -c test002.res.json -f json -g config1 )

  
cet_test(OtsDaqDemo_filesystem_${TEST_NAME}_${TEST_NUMBER}_11_t HANDBUILT  TEST_EXEC OtsDaqDemo_t 
  DATAFILES 
  TEST_ARGS -d filesystem -o load -t test_OtsDaqDemo${TEST_NAME}_${TEST_NUMBER} -c test002.res.json -f json -g config1 )


cet_test(OtsDaqDemo_mongo_${TEST_NAME}_${TEST_NUMBER}_12_t HANDBUILT  TEST_EXEC OtsDaqDemo_t 
  DATAFILES 
  TEST_ARGS -d mongo -o load -t test_OtsDaqDemo${TEST_NAME}_${TEST_NUMBER} -c test002.res.json -f json -g config2 )

  
cet_test(OtsDaqDemo_filesystem_${TEST_NAME}_${TEST_NUMBER}_12_t HANDBUILT  TEST_EXEC OtsDaqDemo_t 
  DATAFILES 
  TEST_ARGS -d filesystem -o load -t test_OtsDaqDemo${TEST_NAME}_${TEST_NUMBER} -c test002.res.json -f json -g config2 )
  