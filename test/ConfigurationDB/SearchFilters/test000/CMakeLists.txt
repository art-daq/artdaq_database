SET ( TEST_NUMBER "000" )
SET ( TEST_NAME "seeddata" )

#conftool -d filesystem -o store -t artdaq_BoardReader -c BoardReader_TOY1_topol.fnal.gov_5205.fcl -f fhicl -v version003 -g testConfigA001 -e BoardReader_TOY1_topol

cet_test(conftool_mongo_store_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC conftool 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5265.fcl 
  TEST_ARGS -d mongo -o store -t artdaq_Aggregator -c Aggregator_topol.fnal.gov_5265.fcl -f fhicl -v version${VERSION_NUMBER} -g testConfigA${CONFIG_NUMBER} -e Aggregator_topol)


cet_test(conftool_mongo_store_${TEST_NUMBER}_02_t HANDBUILT  TEST_EXEC conftool 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/BoardReader_TOY1_topol.fnal.gov_5205.fcl 
  TEST_ARGS -d mongo -o store -t artdaq_BoardReader -c BoardReader_TOY1_topol.fnal.gov_5205.fcl -f fhicl -v version${VERSION_NUMBER} -g testConfigA${CONFIG_NUMBER} -e BoardReader_TOY1_topol)


cet_test(conftool_mongo_store_${TEST_NUMBER}_03_t HANDBUILT  TEST_EXEC conftool 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/BoardReader_TOY2_topol.fnal.gov_5206.fcl 
  TEST_ARGS -d mongo -o store -t artdaq_BoardReader -c BoardReader_TOY2_topol.fnal.gov_5206.fcl -f fhicl -v version${VERSION_NUMBER} -g testConfigA${CONFIG_NUMBER} -e BoardReader_TOY2_topol )


cet_test(conftool_mongo_store_${TEST_NUMBER}_04_t HANDBUILT  TEST_EXEC conftool 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/EventBuilder_topol.fnal.gov_5235.fcl 
  TEST_ARGS -d mongo -o store -t artdaq_EventBuilder -c EventBuilder_topol.fnal.gov_5235.fcl -f fhicl -v version${VERSION_NUMBER} -g testConfigA${CONFIG_NUMBER} -e EventBuilder_topol)
  


cet_test(conftool_filesystem_store_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC conftool 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5265.fcl 
  TEST_ARGS -d filesystem -o store -t artdaq_Aggregator -c Aggregator_topol.fnal.gov_5265.fcl -f fhicl -v version${VERSION_NUMBER} -g testConfigA${CONFIG_NUMBER} -e Aggregator_topol)


cet_test(conftool_filesystem_store_${TEST_NUMBER}_02_t HANDBUILT  TEST_EXEC conftool 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/BoardReader_TOY1_topol.fnal.gov_5205.fcl 
  TEST_ARGS -d filesystem -o store -t artdaq_BoardReader -c BoardReader_TOY1_topol.fnal.gov_5205.fcl -f fhicl -v version${VERSION_NUMBER} -g testConfigA${CONFIG_NUMBER} -e BoardReader_TOY1_topol)


cet_test(conftool_filesystem_store_${TEST_NUMBER}_03_t HANDBUILT  TEST_EXEC conftool 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/BoardReader_TOY2_topol.fnal.gov_5206.fcl 
  TEST_ARGS -d filesystem -o store -t artdaq_BoardReader -c BoardReader_TOY2_topol.fnal.gov_5206.fcl -f fhicl -v version${VERSION_NUMBER} -g testConfigA${CONFIG_NUMBER} -e BoardReader_TOY2_topol )


cet_test(conftool_filesystem_store_${TEST_NUMBER}_04_t HANDBUILT  TEST_EXEC conftool 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/EventBuilder_topol.fnal.gov_5235.fcl 
  TEST_ARGS -d filesystem -o store -t artdaq_EventBuilder -c EventBuilder_topol.fnal.gov_5235.fcl -f fhicl -v version${VERSION_NUMBER} -g testConfigA${CONFIG_NUMBER} -e EventBuilder_topol)
    