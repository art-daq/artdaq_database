SET ( TEST_NUMBER "004" )
SET ( TEST_NAME "addalias" )


configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0041.cmp.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0041.cmp.${RAND_NAME}.json @ONLY )

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0042.cmp.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0042.cmp.${RAND_NAME}.json @ONLY )

cet_test_env("ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_MONGO}b")
  
cet_test(ConfigAliasBuilder_mongo_${TEST_NAME}_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC ConfigurationDB_t 
  DATAFILES 
    ${CMAKE_CURRENT_BINARY_DIR}/test0041.cmp.${RAND_NAME}.json 
  TEST_ARGS  -p mongo -o addveralias -c artdaq_Aggregator -v version001 -a ${TEST_ALIAS_NAME} -f db -x test0041.cmp.${RAND_NAME}.json)

cet_test_env("ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}b")
  
cet_test(ConfigAliasBuilder_filesystem_${TEST_NAME}_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC ConfigurationDB_t 
  DATAFILES 
    ${CMAKE_CURRENT_BINARY_DIR}/test0042.cmp.${RAND_NAME}.json 
  TEST_ARGS  -p filesystem -o addveralias -c artdaq_Aggregator -v version001 -a ${TEST_ALIAS_NAME} -f db -x test0042.cmp.${RAND_NAME}.json)
  
configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0041.flt.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0041.flt.${RAND_NAME}.json @ONLY )

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0042.flt.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0042.flt.${RAND_NAME}.json @ONLY )
  
configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0043.cmp.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0043.cmp.${RAND_NAME}.json @ONLY )

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0044.cmp.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0044.cmp.${RAND_NAME}.json @ONLY )

cet_test(conftool_mongo_${TEST_NAME}_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC conftool 
  DATAFILES 
    ${CMAKE_CURRENT_BINARY_DIR}/test0041.flt.${RAND_NAME}.json 
  TEST_ARGS  --searchquery=test0041.flt.${RAND_NAME}.json -x test0043.cmp.${RAND_NAME}.json)

cet_test(conftool_filesystem_${TEST_NAME}_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC conftool 
  DATAFILES 
   ${CMAKE_CURRENT_BINARY_DIR}/test0042.flt.${RAND_NAME}.json 
  TEST_ARGS  --searchquery=test0042.flt.${RAND_NAME}.json -x test0044.cmp.${RAND_NAME}.json)  