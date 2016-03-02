SET ( TEST_NUMBER "008" )
SET ( TEST_NAME "load-store" )
SET ( VERSION_NUMBER "001")
SET ( CONFIG_NUMBER "003")



cet_test(ArtdaqDemo_filesystem_load_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC ArtdaqDemo_t 
  DATAFILES 
  TEST_ARGS -d filesystem -o load -t artdaq_Aggregator1 -c Aggregator_topol.fnal.gov_5265.fcl -f fhicl  -g config-${CONFIG_NUMBER} )


cet_test(ArtdaqDemo_filesystem_load_${TEST_NUMBER}_02_t HANDBUILT  TEST_EXEC ArtdaqDemo_t 
  DATAFILES 
  TEST_ARGS -d filesystem -o load -t artdaq_Aggregator2 -c Aggregator_topol.fnal.gov_5266.fcl -f fhicl  -g config-${CONFIG_NUMBER} )


cet_test(ArtdaqDemo_filesystem_load_${TEST_NUMBER}_03_t HANDBUILT  TEST_EXEC ArtdaqDemo_t 
  DATAFILES 
  TEST_ARGS -d filesystem -o load -t artdaq_BoardReader1 -c BoardReader_TOY1_topol.fnal.gov_5205.fcl -f fhicl -g config-${CONFIG_NUMBER} )


cet_test(ArtdaqDemo_filesystem_load_${TEST_NUMBER}_04_t HANDBUILT  TEST_EXEC ArtdaqDemo_t 
  DATAFILES 
  TEST_ARGS -d filesystem -o load -t artdaq_BoardReader2 -c BoardReader_TOY2_topol.fnal.gov_5206.fcl -f fhicl  -g config-${CONFIG_NUMBER} )


cet_test(ArtdaqDemo_filesystem_load_${TEST_NUMBER}_05_t HANDBUILT  TEST_EXEC ArtdaqDemo_t 
  DATAFILES 
  TEST_ARGS -d filesystem -o load -t artdaq_EventBuilder1 -c EventBuilder_topol.fnal.gov_5235.fcl -f fhicl  -g config-${CONFIG_NUMBER} )
  
cet_test(ArtdaqDemo_filesystem_load_${TEST_NUMBER}_06_t HANDBUILT  TEST_EXEC ArtdaqDemo_t 
  DATAFILES 
  TEST_ARGS -d filesystem -o load -t artdaq_EventBuilder2 -c EventBuilder_topol.fnal.gov_5236.fcl -f fhicl  -g config-${CONFIG_NUMBER} )


  
cet_test(ArtdaqDemo_filesystem_load_${TEST_NUMBER}_11_t HANDBUILT  TEST_EXEC ArtdaqDemo_t 
  DATAFILES 
  TEST_ARGS -d mongo -o load -t artdaq_Aggregator1 -c Aggregator_topol.fnal.gov_5265.fcl -f fhicl  -g config-${CONFIG_NUMBER} )


cet_test(ArtdaqDemo_filesystem_load_${TEST_NUMBER}_12_t HANDBUILT  TEST_EXEC ArtdaqDemo_t 
  DATAFILES 
  TEST_ARGS -d mongo -o load -t artdaq_Aggregator2 -c Aggregator_topol.fnal.gov_5266.fcl -f fhicl  -g config-${CONFIG_NUMBER} )


cet_test(ArtdaqDemo_filesystem_load_${TEST_NUMBER}_13_t HANDBUILT  TEST_EXEC ArtdaqDemo_t 
  DATAFILES 
  TEST_ARGS -d mongo -o load -t artdaq_BoardReader1 -c BoardReader_TOY1_topol.fnal.gov_5205.fcl -f fhicl  -g config-${CONFIG_NUMBER} )


cet_test(ArtdaqDemo_filesystem_load_${TEST_NUMBER}_14_t HANDBUILT  TEST_EXEC ArtdaqDemo_t 
  DATAFILES 
  TEST_ARGS -d mongo -o load -t artdaq_BoardReader2 -c BoardReader_TOY2_topol.fnal.gov_5206.fcl -f fhicl  -g config-${CONFIG_NUMBER} )


cet_test(ArtdaqDemo_filesystem_load_${TEST_NUMBER}_15_t HANDBUILT  TEST_EXEC ArtdaqDemo_t 
  DATAFILES 
  TEST_ARGS -d mongo -o load -t artdaq_EventBuilder1 -c EventBuilder_topol.fnal.gov_5235.fcl -f fhicl  -g config-${CONFIG_NUMBER} )
  
cet_test(ArtdaqDemo_filesystem_load_${TEST_NUMBER}_16_t HANDBUILT  TEST_EXEC ArtdaqDemo_t 
  DATAFILES 
  TEST_ARGS -d mongo -o load -t artdaq_EventBuilder2 -c EventBuilder_topol.fnal.gov_5236.fcl -f fhicl  -g config-${CONFIG_NUMBER} )

    