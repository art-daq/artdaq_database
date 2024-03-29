SET ( TEST_NUMBER "008" )

SET ( TEST_NAME "ArtdaqDemo_load" )
SET ( TEST_DBPROVIDER "filesystem" )
cet_test_env("ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}")


cet_test(${TEST_NAME}_${TEST_DBPROVIDER}_008_01_t HANDBUILT  TEST_EXEC ArtdaqDemo_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5265.cmp.fcl    
  TEST_ARGS -p ${TEST_DBPROVIDER} -o load -c artdaq_Aggregator1  -x Aggregator_topol.fnal.gov_5265.cmp.fcl -f fhicl -g ${TEST_CONFIGURATION_NAME}-007 )


cet_test(${TEST_NAME}_${TEST_DBPROVIDER}_008_02_t HANDBUILT  TEST_EXEC ArtdaqDemo_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5266.cmp.fcl     
  TEST_ARGS -p ${TEST_DBPROVIDER} -o load -c artdaq_Aggregator2  -x Aggregator_topol.fnal.gov_5266.cmp.fcl -f fhicl -g ${TEST_CONFIGURATION_NAME}-007 )


cet_test(${TEST_NAME}_${TEST_DBPROVIDER}_008_03_t HANDBUILT  TEST_EXEC ArtdaqDemo_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/BoardReader_TOY1_topol.fnal.gov_5205.cmp.fcl    
  TEST_ARGS -p ${TEST_DBPROVIDER} -o load -c artdaq_BoardReader1  -x BoardReader_TOY1_topol.fnal.gov_5205.cmp.fcl -f fhicl -g ${TEST_CONFIGURATION_NAME}-007 )


cet_test(${TEST_NAME}_${TEST_DBPROVIDER}_008_04_t HANDBUILT  TEST_EXEC ArtdaqDemo_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/BoardReader_TOY2_topol.fnal.gov_5206.cmp.fcl     
  TEST_ARGS -p ${TEST_DBPROVIDER} -o load -c artdaq_BoardReader2  -x BoardReader_TOY2_topol.fnal.gov_5206.cmp.fcl -f fhicl -g ${TEST_CONFIGURATION_NAME}-007 )


cet_test(${TEST_NAME}_${TEST_DBPROVIDER}_008_05_t HANDBUILT  TEST_EXEC ArtdaqDemo_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/EventBuilder_topol.fnal.gov_5235.cmp.fcl    
  TEST_ARGS -p ${TEST_DBPROVIDER} -o load -c artdaq_EventBuilder1  -x EventBuilder_topol.fnal.gov_5235.cmp.fcl -f fhicl -g ${TEST_CONFIGURATION_NAME}-007 )
  
cet_test(${TEST_NAME}_${TEST_DBPROVIDER}_008_06_t HANDBUILT  TEST_EXEC ArtdaqDemo_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/EventBuilder_topol.fnal.gov_5236.cmp.fcl     
  TEST_ARGS -p ${TEST_DBPROVIDER} -o load -c artdaq_EventBuilder2  -x EventBuilder_topol.fnal.gov_5236.cmp.fcl -f fhicl -g ${TEST_CONFIGURATION_NAME}-007 )

SET ( TEST_DBPROVIDER "mongo" )
cet_test_env("ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_MONGO}")
  
cet_test(${TEST_NAME}_${TEST_DBPROVIDER}_008_11_t HANDBUILT  TEST_EXEC ArtdaqDemo_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5265.cmp.fcl    
  TEST_ARGS -p ${TEST_DBPROVIDER} -o load -c artdaq_Aggregator1  -x Aggregator_topol.fnal.gov_5265.cmp.fcl -f fhicl -g ${TEST_CONFIGURATION_NAME}-007 )


cet_test(${TEST_NAME}_${TEST_DBPROVIDER}_008_12_t HANDBUILT  TEST_EXEC ArtdaqDemo_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5266.cmp.fcl     
  TEST_ARGS -p ${TEST_DBPROVIDER} -o load -c artdaq_Aggregator2  -x Aggregator_topol.fnal.gov_5266.cmp.fcl -f fhicl -g ${TEST_CONFIGURATION_NAME}-007 )


cet_test(${TEST_NAME}_${TEST_DBPROVIDER}_008_13_t HANDBUILT  TEST_EXEC ArtdaqDemo_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/BoardReader_TOY1_topol.fnal.gov_5205.cmp.fcl    
  TEST_ARGS -p ${TEST_DBPROVIDER} -o load -c artdaq_BoardReader1  -x BoardReader_TOY1_topol.fnal.gov_5205.cmp.fcl -f fhicl -g ${TEST_CONFIGURATION_NAME}-007 )


cet_test(${TEST_NAME}_${TEST_DBPROVIDER}_008_14_t HANDBUILT  TEST_EXEC ArtdaqDemo_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/BoardReader_TOY2_topol.fnal.gov_5206.cmp.fcl     
  TEST_ARGS -p ${TEST_DBPROVIDER} -o load -c artdaq_BoardReader2  -x BoardReader_TOY2_topol.fnal.gov_5206.cmp.fcl -f fhicl -g ${TEST_CONFIGURATION_NAME}-007 )


cet_test(${TEST_NAME}_${TEST_DBPROVIDER}_008_15_t HANDBUILT  TEST_EXEC ArtdaqDemo_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/EventBuilder_topol.fnal.gov_5235.cmp.fcl    
  TEST_ARGS -p ${TEST_DBPROVIDER} -o load -c artdaq_EventBuilder1  -x EventBuilder_topol.fnal.gov_5235.cmp.fcl -f fhicl -g ${TEST_CONFIGURATION_NAME}-007 )
  
cet_test(${TEST_NAME}_${TEST_DBPROVIDER}_008_16_t HANDBUILT  TEST_EXEC ArtdaqDemo_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/EventBuilder_topol.fnal.gov_5236.cmp.fcl     
  TEST_ARGS -p ${TEST_DBPROVIDER} -o load -c artdaq_EventBuilder2  -x EventBuilder_topol.fnal.gov_5236.cmp.fcl -f fhicl -g ${TEST_CONFIGURATION_NAME}-007 )
    