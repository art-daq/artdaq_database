SET ( TEST_NUMBER "007" )

SET ( TEST_NAME "ArtdaqDemo_store" )
SET ( TEST_DBPROVIDER "filesystem" )
cet_test_env("ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}")


cet_test(${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC ArtdaqDemo_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5265.src.fcl
    ${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5265.cmp.fcl    
  TEST_ARGS -p ${TEST_DBPROVIDER} -o store -c artdaq_Aggregator1 -s Aggregator_topol.fnal.gov_5265.src.fcl -x Aggregator_topol.fnal.gov_5265.cmp.fcl -f fhicl -v ${TEST_VERSION_NAME} -g ${TEST_CONFIGURATION_NAME}-${TEST_NUMBER} )


cet_test(${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_02_t HANDBUILT  TEST_EXEC ArtdaqDemo_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5266.src.fcl 
    ${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5266.cmp.fcl     
  TEST_ARGS -p ${TEST_DBPROVIDER} -o store -c artdaq_Aggregator2 -s Aggregator_topol.fnal.gov_5266.src.fcl -x Aggregator_topol.fnal.gov_5266.cmp.fcl -f fhicl -v ${TEST_VERSION_NAME} -g ${TEST_CONFIGURATION_NAME}-${TEST_NUMBER} )


cet_test(${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_03_t HANDBUILT  TEST_EXEC ArtdaqDemo_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/BoardReader_TOY1_topol.fnal.gov_5205.src.fcl
    ${CMAKE_CURRENT_SOURCE_DIR}/BoardReader_TOY1_topol.fnal.gov_5205.cmp.fcl    
  TEST_ARGS -p ${TEST_DBPROVIDER} -o store -c artdaq_BoardReader1 -s BoardReader_TOY1_topol.fnal.gov_5205.src.fcl -x BoardReader_TOY1_topol.fnal.gov_5205.cmp.fcl -f fhicl -v ${TEST_VERSION_NAME} -g ${TEST_CONFIGURATION_NAME}-${TEST_NUMBER} )


cet_test(${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_04_t HANDBUILT  TEST_EXEC ArtdaqDemo_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/BoardReader_TOY2_topol.fnal.gov_5206.src.fcl
    ${CMAKE_CURRENT_SOURCE_DIR}/BoardReader_TOY2_topol.fnal.gov_5206.cmp.fcl     
  TEST_ARGS -p ${TEST_DBPROVIDER} -o store -c artdaq_BoardReader2 -s BoardReader_TOY2_topol.fnal.gov_5206.src.fcl -x BoardReader_TOY2_topol.fnal.gov_5206.cmp.fcl -f fhicl -v ${TEST_VERSION_NAME} -g ${TEST_CONFIGURATION_NAME}-${TEST_NUMBER} )


cet_test(${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_05_t HANDBUILT  TEST_EXEC ArtdaqDemo_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/EventBuilder_topol.fnal.gov_5235.src.fcl
    ${CMAKE_CURRENT_SOURCE_DIR}/EventBuilder_topol.fnal.gov_5235.cmp.fcl    
  TEST_ARGS -p ${TEST_DBPROVIDER} -o store -c artdaq_EventBuilder1 -s EventBuilder_topol.fnal.gov_5235.src.fcl -x EventBuilder_topol.fnal.gov_5235.cmp.fcl -f fhicl -v ${TEST_VERSION_NAME} -g ${TEST_CONFIGURATION_NAME}-${TEST_NUMBER} )
  
cet_test(${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_06_t HANDBUILT  TEST_EXEC ArtdaqDemo_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/EventBuilder_topol.fnal.gov_5236.src.fcl
    ${CMAKE_CURRENT_SOURCE_DIR}/EventBuilder_topol.fnal.gov_5236.cmp.fcl     
  TEST_ARGS -p ${TEST_DBPROVIDER} -o store -c artdaq_EventBuilder2 -s EventBuilder_topol.fnal.gov_5236.src.fcl -x EventBuilder_topol.fnal.gov_5236.cmp.fcl -f fhicl -v ${TEST_VERSION_NAME} -g ${TEST_CONFIGURATION_NAME}-${TEST_NUMBER} )

SET ( TEST_DBPROVIDER "mongo" )
cet_test_env("ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_MONGO}")
  
cet_test(${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_11_t HANDBUILT  TEST_EXEC ArtdaqDemo_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5265.src.fcl
    ${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5265.cmp.fcl    
  TEST_ARGS -p ${TEST_DBPROVIDER} -o store -c artdaq_Aggregator1 -s Aggregator_topol.fnal.gov_5265.src.fcl -x Aggregator_topol.fnal.gov_5265.cmp.fcl -f fhicl -v ${TEST_VERSION_NAME} -g ${TEST_CONFIGURATION_NAME}-${TEST_NUMBER} )


cet_test(${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_12_t HANDBUILT  TEST_EXEC ArtdaqDemo_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5266.src.fcl 
    ${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5266.cmp.fcl     
  TEST_ARGS -p ${TEST_DBPROVIDER} -o store -c artdaq_Aggregator2 -s Aggregator_topol.fnal.gov_5266.src.fcl -x Aggregator_topol.fnal.gov_5266.cmp.fcl -f fhicl -v ${TEST_VERSION_NAME} -g ${TEST_CONFIGURATION_NAME}-${TEST_NUMBER} )


cet_test(${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_13_t HANDBUILT  TEST_EXEC ArtdaqDemo_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/BoardReader_TOY1_topol.fnal.gov_5205.src.fcl
    ${CMAKE_CURRENT_SOURCE_DIR}/BoardReader_TOY1_topol.fnal.gov_5205.cmp.fcl    
  TEST_ARGS -p ${TEST_DBPROVIDER} -o store -c artdaq_BoardReader1 -s BoardReader_TOY1_topol.fnal.gov_5205.src.fcl -x BoardReader_TOY1_topol.fnal.gov_5205.cmp.fcl -f fhicl -v ${TEST_VERSION_NAME} -g ${TEST_CONFIGURATION_NAME}-${TEST_NUMBER} )


cet_test(${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_14_t HANDBUILT  TEST_EXEC ArtdaqDemo_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/BoardReader_TOY2_topol.fnal.gov_5206.src.fcl
    ${CMAKE_CURRENT_SOURCE_DIR}/BoardReader_TOY2_topol.fnal.gov_5206.cmp.fcl     
  TEST_ARGS -p ${TEST_DBPROVIDER} -o store -c artdaq_BoardReader2 -s BoardReader_TOY2_topol.fnal.gov_5206.src.fcl -x BoardReader_TOY2_topol.fnal.gov_5206.cmp.fcl -f fhicl -v ${TEST_VERSION_NAME} -g ${TEST_CONFIGURATION_NAME}-${TEST_NUMBER} )


cet_test(${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_15_t HANDBUILT  TEST_EXEC ArtdaqDemo_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/EventBuilder_topol.fnal.gov_5235.src.fcl
    ${CMAKE_CURRENT_SOURCE_DIR}/EventBuilder_topol.fnal.gov_5235.cmp.fcl    
  TEST_ARGS -p ${TEST_DBPROVIDER} -o store -c artdaq_EventBuilder1 -s EventBuilder_topol.fnal.gov_5235.src.fcl -x EventBuilder_topol.fnal.gov_5235.cmp.fcl -f fhicl -v ${TEST_VERSION_NAME} -g ${TEST_CONFIGURATION_NAME}-${TEST_NUMBER} )
  
cet_test(${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_16_t HANDBUILT  TEST_EXEC ArtdaqDemo_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/EventBuilder_topol.fnal.gov_5236.src.fcl
    ${CMAKE_CURRENT_SOURCE_DIR}/EventBuilder_topol.fnal.gov_5236.cmp.fcl     
  TEST_ARGS -p ${TEST_DBPROVIDER} -o store -c artdaq_EventBuilder2 -s EventBuilder_topol.fnal.gov_5236.src.fcl -x EventBuilder_topol.fnal.gov_5236.cmp.fcl -f fhicl -v ${TEST_VERSION_NAME} -g ${TEST_CONFIGURATION_NAME}-${TEST_NUMBER} )
    