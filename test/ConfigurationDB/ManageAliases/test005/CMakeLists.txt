SET ( TEST_NUMBER "005" )
SET ( TEST_NAME "rmalias" )


configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0051.cmp.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0051.cmp.${RAND_NAME}.json @ONLY )

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0052.cmp.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0052.cmp.${RAND_NAME}.json @ONLY )

cet_test_env("ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_MONGO}b")
  
cet_test(ConfigAliasBuilder_mongo_${TEST_NAME}_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC ConfigurationDB_t 
  DATAFILES 
    ${CMAKE_CURRENT_BINARY_DIR}/test0051.cmp.${RAND_NAME}.json 
  TEST_ARGS  -p mongo -o rmveralias -c artdaq_Aggregator -v version001 -a ${TEST_ALIAS_NAME} -f db -x test0051.cmp.${RAND_NAME}.json)

cet_test_env("ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}b")
  
cet_test(ConfigAliasBuilder_filesystem_${TEST_NAME}_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC ConfigurationDB_t 
  DATAFILES 
    ${CMAKE_CURRENT_BINARY_DIR}/test0052.cmp.${RAND_NAME}.json 
  TEST_ARGS  -p filesystem -o rmveralias -c artdaq_Aggregator -v version001 -a ${TEST_ALIAS_NAME} -f db -x test0052.cmp.${RAND_NAME}.json)
  
configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0051.flt.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0051.flt.${RAND_NAME}.json @ONLY )

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0052.flt.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0052.flt.${RAND_NAME}.json @ONLY )
  
configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0053.cmp.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0053.cmp.${RAND_NAME}.json @ONLY )

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0054.cmp.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0054.cmp.${RAND_NAME}.json @ONLY )

cet_test(conftool_mongo_${TEST_NAME}_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC conftool 
  DATAFILES 
    ${CMAKE_CURRENT_BINARY_DIR}/test0051.flt.${RAND_NAME}.json 
  TEST_ARGS  --searchquery=test0051.flt.${RAND_NAME}.json -x test0053.cmp.${RAND_NAME}.json)

cet_test(conftool_filesystem_${TEST_NAME}_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC conftool 
  DATAFILES 
   ${CMAKE_CURRENT_BINARY_DIR}/test0052.flt.${RAND_NAME}.json 
  TEST_ARGS  --searchquery=test0052.flt.${RAND_NAME}.json -x test0054.cmp.${RAND_NAME}.json)  