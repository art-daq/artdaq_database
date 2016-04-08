SET ( TEST_NUMBER "000" )
SET ( TEST_NAME "seeddatagcb" )

cet_test(conftool_mongo_store_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC conftool 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5265.fcl 
  TEST_ARGS -d mongo -o store -t artdaq_Aggregator -c Aggregator_topol.fnal.gov_5265.fcl -f fhicl -v version004 -e ${TEST_ENTITY_NAME})

cet_test(conftool_mongo_store_${TEST_NUMBER}_02_t HANDBUILT  TEST_EXEC conftool 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5266.fcl 
  TEST_ARGS -d mongo -o store -t artdaq_Aggregator -c Aggregator_topol.fnal.gov_5266.fcl -f fhicl -v version002 -e ${TEST_ENTITY_NAME})

cet_test(conftool_mongo_store_${TEST_NUMBER}_03_t HANDBUILT  TEST_EXEC conftool 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5265.fcl 
  TEST_ARGS -d mongo -o store -t artdaq_Aggregator -c Aggregator_topol.fnal.gov_5265.fcl -f fhicl -v version003 -e ${TEST_ENTITY_NAME})

cet_test(conftool_mongo_store_${TEST_NUMBER}_04_t HANDBUILT  TEST_EXEC conftool 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5266.fcl 
  TEST_ARGS -d mongo -o store -t artdaq_Aggregator -c Aggregator_topol.fnal.gov_5266.fcl -f fhicl -v version001 -e ${TEST_ENTITY_NAME})


  
  
  

  
cet_test(conftool_filesystem_store_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC conftool 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5265.fcl 
  TEST_ARGS -d filesystem -o store -t artdaq_Aggregator -c Aggregator_topol.fnal.gov_5265.fcl -f fhicl -v version004 -e ${TEST_ENTITY_NAME})

cet_test(conftool_filesystem_store_${TEST_NUMBER}_02_t HANDBUILT  TEST_EXEC conftool 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5266.fcl 
  TEST_ARGS -d filesystem -o store -t artdaq_Aggregator -c Aggregator_topol.fnal.gov_5266.fcl -f fhicl -v version002 -e ${TEST_ENTITY_NAME})

cet_test(conftool_filesystem_store_${TEST_NUMBER}_03_t HANDBUILT  TEST_EXEC conftool 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5265.fcl 
  TEST_ARGS -d filesystem -o store -t artdaq_Aggregator -c Aggregator_topol.fnal.gov_5265.fcl -f fhicl -v version003 -e ${TEST_ENTITY_NAME})

cet_test(conftool_filesystem_store_${TEST_NUMBER}_04_t HANDBUILT  TEST_EXEC conftool 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5266.fcl 
  TEST_ARGS -d filesystem -o store -t artdaq_Aggregator -c Aggregator_topol.fnal.gov_5266.fcl -f fhicl -v version001 -e ${TEST_ENTITY_NAME})
  
