SET ( TEST_NUMBER "005" )
SET ( TEST_NAME "load" )

cet_test(ArtdaqDemo_filesystem_${TEST_NAME}_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC ArtdaqDemo_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test005.src.fcl 
  TEST_ARGS -d filesystem -o store -t test_${TEST_NAME}_${TEST_NUMBER} -c test005.src.fcl -f fhicl -v version -g config-${TEST_NUMBER} )

cet_test(ArtdaqDemo_filesystem_${TEST_NAME}_${TEST_NUMBER}_02_t HANDBUILT  TEST_EXEC ArtdaqDemo_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test005.cmp.fcl 
  TEST_ARGS -d filesystem -o load -t test_${TEST_NAME}_${TEST_NUMBER} -c test005.out.fcl -f fhicl -g config-${TEST_NUMBER} )
  