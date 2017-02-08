SET ( TEST_NUMBER "001" )
SET ( TEST_NAME "findversions" )

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0011.cmp.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0011.cmp.${RAND_NAME}.json @ONLY )

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0012.cmp.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0012.cmp.${RAND_NAME}.json @ONLY )

  
cet_test_env("ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_MONGO}b")
cet_test(ConfigAliasBuilder_mongo_${TEST_NAME}_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC ConfigAliasBuilder_t 
  DATAFILES 
    ${CMAKE_CURRENT_BINARY_DIR}/test0011.cmp.${RAND_NAME}.json 
  TEST_ARGS  -p mongo -o findversions -f gui -e ${TEST_ENTITY_NAME} -c artdaq_Aggregator -x test0011.cmp.${RAND_NAME}.json)

cet_test_env("ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}b")
cet_test(ConfigAliasBuilder_filesystem_${TEST_NAME}_${TEST_NUMBER}_02_t HANDBUILT  TEST_EXEC ConfigAliasBuilder_t 
  DATAFILES 
    ${CMAKE_CURRENT_BINARY_DIR}/test0012.cmp.${RAND_NAME}.json 
  TEST_ARGS  -p filesystem -o findversions -f gui -e ${TEST_ENTITY_NAME} -c artdaq_Aggregator -x test0012.cmp.${RAND_NAME}.json)

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0011.flt.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0011.flt.${RAND_NAME}.json @ONLY )

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0012.flt.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0012.flt.${RAND_NAME}.json @ONLY )
  
cet_test_env("ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_MONGO}b")
cet_test(conftool_mongo_${TEST_NAME}_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC conftool 
  DATAFILES 
    ${CMAKE_CURRENT_BINARY_DIR}/test0011.flt.${RAND_NAME}.json
    ${CMAKE_CURRENT_BINARY_DIR}/test0011.cmp.${RAND_NAME}.json    
  TEST_ARGS  --searchquery=test0011.flt.${RAND_NAME}.json -x test0011.cmp.${RAND_NAME}.json)

cet_test_env("ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}b")
cet_test(conftool_filesystem_${TEST_NAME}_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC conftool 
  DATAFILES 
    ${CMAKE_CURRENT_BINARY_DIR}/test0012.flt.${RAND_NAME}.json
    ${CMAKE_CURRENT_BINARY_DIR}/test0012.cmp.${RAND_NAME}.json
  TEST_ARGS  --searchquery=test0012.flt.${RAND_NAME}.json -x test0012.cmp.${RAND_NAME}.json)
