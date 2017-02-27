SET ( TEST_NUMBER "003" )
SET ( TEST_NAME "findentities" )


configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0031.cmp.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0031.cmp.${RAND_NAME}.json @ONLY )

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0032.cmp.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0032.cmp.${RAND_NAME}.json @ONLY )

cet_test_env("ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_MONGO}b")
  
cet_test(ConfigAliasBuilder_mongo_${TEST_NAME}_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC ConfigurationDB_t 
  DATAFILES 
    ${CMAKE_CURRENT_BINARY_DIR}/test0031.cmp.${RAND_NAME}.json 
  TEST_ARGS  -p mongo -o findentities -f gui -x test0031.cmp.${RAND_NAME}.json)

cet_test_env("ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}b")
  
cet_test(ConfigAliasBuilder_filesystem_${TEST_NAME}_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC ConfigurationDB_t 
  DATAFILES 
    ${CMAKE_CURRENT_BINARY_DIR}/test0032.cmp.${RAND_NAME}.json 
  TEST_ARGS  -p filesystem -o findentities -f gui -x test0032.cmp.${RAND_NAME}.json)
  
configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0031.flt.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0031.flt.${RAND_NAME}.json @ONLY )

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0032.flt.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0032.flt.${RAND_NAME}.json @ONLY )
  
configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0033.cmp.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0033.cmp.${RAND_NAME}.json @ONLY )

configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/test0034.cmp.json
  ${CMAKE_CURRENT_BINARY_DIR}/test0034.cmp.${RAND_NAME}.json @ONLY )

cet_test(conftool_mongo_${TEST_NAME}_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC conftool 
  DATAFILES 
    ${CMAKE_CURRENT_BINARY_DIR}/test0031.flt.${RAND_NAME}.json 
  TEST_ARGS  --searchquery=test0031.flt.${RAND_NAME}.json -x test0033.cmp.${RAND_NAME}.json)

cet_test(conftool_filesystem_${TEST_NAME}_${TEST_NUMBER}_01_t HANDBUILT  TEST_EXEC conftool 
  DATAFILES 
   ${CMAKE_CURRENT_BINARY_DIR}/test0032.flt.${RAND_NAME}.json 
  TEST_ARGS  --searchquery=test0032.flt.${RAND_NAME}.json -x test0034.cmp.${RAND_NAME}.json)  