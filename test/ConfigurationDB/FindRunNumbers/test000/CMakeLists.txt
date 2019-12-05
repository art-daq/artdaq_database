SET ( TEST_NUMBER "000" )
SET ( TEST_NAME "seeddatafindruns" )

cet_test_env("ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_MONGO}_archive")

cet_test(conftool_mongo_findruns_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC conftool 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5265.fcl 
		TEST_ARGS -p mongo -o store -c artdaq_Aggregator -s Aggregator_topol.fnal.gov_5265.fcl -f fhicl -v version001
		-e ${TEST_ENTITY_NAME}a -r 100${TEST_RUN_NUMBER} -g ${TEST_CONFIG_NAME}a 
		)

cet_test(conftool_mongo_findruns_${TEST_NUMBER}_02_t HANDBUILT  TEST_EXEC conftool 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5266.fcl 
		TEST_ARGS -p mongo -o store -c artdaq_Aggregator -s Aggregator_topol.fnal.gov_5266.fcl -f fhicl -v version001 
		-e ${TEST_ENTITY_NAME}b -r 100${TEST_RUN_NUMBER} -g ${TEST_CONFIG_NAME}a 
		)

cet_test(conftool_mongo_findruns_${TEST_NUMBER}_03_t HANDBUILT  TEST_EXEC conftool 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5265.fcl 
		TEST_ARGS -p mongo -o store -c artdaq_Aggregator -s Aggregator_topol.fnal.gov_5265.fcl -f fhicl -v version002
		-e ${TEST_ENTITY_NAME}a -r 200${TEST_RUN_NUMBER} -g ${TEST_CONFIG_NAME}b 
		)

cet_test(conftool_mongo_findruns_${TEST_NUMBER}_04_t HANDBUILT  TEST_EXEC conftool 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5266.fcl 
		TEST_ARGS -p mongo -o store -c artdaq_Aggregator -s Aggregator_topol.fnal.gov_5266.fcl -f fhicl -v version002
		-e ${TEST_ENTITY_NAME}b -r 200${TEST_RUN_NUMBER} -g ${TEST_CONFIG_NAME}b 
		) 


cet_test_env("ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}_archive")
  
cet_test(conftool_filesystem_findruns_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC conftool 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5265.fcl 
		TEST_ARGS -p filesystem -o store -c artdaq_Aggregator -s Aggregator_topol.fnal.gov_5265.fcl -f fhicl -v version001
		-e ${TEST_ENTITY_NAME}a -r 100${TEST_RUN_NUMBER} -g ${TEST_CONFIG_NAME}a 
		)

cet_test(conftool_filesystem_findruns_${TEST_NUMBER}_02_t HANDBUILT  TEST_EXEC conftool 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5266.fcl 
		TEST_ARGS -p filesystem -o store -c artdaq_Aggregator -s Aggregator_topol.fnal.gov_5266.fcl -f fhicl -v version001 
		-e ${TEST_ENTITY_NAME}b -r 100${TEST_RUN_NUMBER} -g ${TEST_CONFIG_NAME}a 
		)

cet_test(conftool_filesystem_findruns_${TEST_NUMBER}_03_t HANDBUILT  TEST_EXEC conftool 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5265.fcl 
		TEST_ARGS -p filesystem -o store -c artdaq_Aggregator -s Aggregator_topol.fnal.gov_5265.fcl -f fhicl -v version002
		-e ${TEST_ENTITY_NAME}a -r 200${TEST_RUN_NUMBER} -g ${TEST_CONFIG_NAME}b 
		)

cet_test(conftool_filesystem_findruns_${TEST_NUMBER}_04_t HANDBUILT  TEST_EXEC conftool 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/Aggregator_topol.fnal.gov_5266.fcl 
		TEST_ARGS -p filesystem -o store -c artdaq_Aggregator -s Aggregator_topol.fnal.gov_5266.fcl -f fhicl -v version002
		-e ${TEST_ENTITY_NAME}b -r 200${TEST_RUN_NUMBER} -g ${TEST_CONFIG_NAME}b 
		) 

  
