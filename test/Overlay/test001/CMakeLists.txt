cet_make_exec(NAME JSONOverlay_t
  LIBRARIES
  
  artdaq-database_DataFormats_Json
  
  artdaq-database_Overlay
  cetlib
  cetlib_except
  Boost::program_options
) 

SET ( TEST_NAME "compare" )

SET ( TEST_NUMBER "001" )

SET ( SUBTEST_NUMBER "a" )
cet_test(JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t HANDBUILT  TEST_EXEC JSONOverlay_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json 
  TEST_ARGS -m 3 -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json )

SET ( SUBTEST_NUMBER "b" )
cet_test(JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t HANDBUILT  TEST_EXEC JSONOverlay_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json 
  TEST_ARGS -f -m 1 -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json )

SET ( TEST_NUMBER "002" )

SET ( SUBTEST_NUMBER "a" )
cet_test(JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t HANDBUILT  TEST_EXEC JSONOverlay_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json 
  TEST_ARGS -f -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json )
  
SET ( SUBTEST_NUMBER "b" )
  cet_test(JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t HANDBUILT  TEST_EXEC JSONOverlay_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json 
  TEST_ARGS -m 1 -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json )

SET ( SUBTEST_NUMBER "c" )
  cet_test(JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t HANDBUILT  TEST_EXEC JSONOverlay_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json 
  TEST_ARGS -m 8 -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json )  
  
SET ( TEST_NUMBER "003" )
SET ( SUBTEST_NUMBER "a" )
cet_test(JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t HANDBUILT  TEST_EXEC JSONOverlay_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json 
  TEST_ARGS -f -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json )

SET ( SUBTEST_NUMBER "b" )
cet_test(JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t HANDBUILT  TEST_EXEC JSONOverlay_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json 
  TEST_ARGS -m 8 -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json )

SET ( TEST_NUMBER "004" )
SET ( SUBTEST_NUMBER "a" )
cet_test(JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t HANDBUILT  TEST_EXEC JSONOverlay_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json 
  TEST_ARGS -f -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json )

SET ( SUBTEST_NUMBER "b" )
cet_test(JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t HANDBUILT  TEST_EXEC JSONOverlay_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json 
  TEST_ARGS -m 8 -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json )

SET ( TEST_NUMBER "005" )
SET ( SUBTEST_NUMBER "a" )
cet_test(JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t HANDBUILT  TEST_EXEC JSONOverlay_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json 
  TEST_ARGS -f -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json )

SET ( SUBTEST_NUMBER "b" )
cet_test(JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t HANDBUILT  TEST_EXEC JSONOverlay_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json 
  TEST_ARGS -m 128 -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json )


SET ( TEST_NUMBER "006" )
SET ( SUBTEST_NUMBER "a" )
cet_test(JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t HANDBUILT  TEST_EXEC JSONOverlay_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json 
  TEST_ARGS -f -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json )

SET ( SUBTEST_NUMBER "b" )
cet_test(JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t HANDBUILT  TEST_EXEC JSONOverlay_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json 
  TEST_ARGS -m 536870912 -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json )  

  
SET ( TEST_NUMBER "007" )
SET ( SUBTEST_NUMBER "a" )
cet_test(JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t HANDBUILT  TEST_EXEC JSONOverlay_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json 
  TEST_ARGS -f -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json )

SET ( SUBTEST_NUMBER "b" )
cet_test(JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t HANDBUILT  TEST_EXEC JSONOverlay_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json 
  TEST_ARGS -m 1073741824 -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json )    

  SET ( TEST_NUMBER "008" )
SET ( SUBTEST_NUMBER "a" )
cet_test(JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t HANDBUILT  TEST_EXEC JSONOverlay_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json 
  TEST_ARGS -f -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json )

SET ( SUBTEST_NUMBER "b" )
cet_test(JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t HANDBUILT  TEST_EXEC JSONOverlay_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json 
  TEST_ARGS -m 16 -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json )      


SET ( TEST_NUMBER "009" )
SET ( SUBTEST_NUMBER "a" )
cet_test(JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t HANDBUILT  TEST_EXEC JSONOverlay_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json 
  TEST_ARGS -f -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json )

SET ( SUBTEST_NUMBER "b" )
cet_test(JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t HANDBUILT  TEST_EXEC JSONOverlay_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json 
  TEST_ARGS -m 32 -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json )      

SET ( TEST_NUMBER "010" )
SET ( SUBTEST_NUMBER "a" )
cet_test(JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t HANDBUILT  TEST_EXEC JSONOverlay_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json 
  TEST_ARGS -f -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json )

SET ( SUBTEST_NUMBER "b" )
cet_test(JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t HANDBUILT  TEST_EXEC JSONOverlay_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json 
  TEST_ARGS -m 512 -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json )    

SET ( TEST_NUMBER "011" )
SET ( SUBTEST_NUMBER "a" )
cet_test(JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t HANDBUILT  TEST_EXEC JSONOverlay_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json 
  TEST_ARGS -f -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json )

SET ( SUBTEST_NUMBER "b" )
cet_test(JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t HANDBUILT  TEST_EXEC JSONOverlay_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json 
  TEST_ARGS -m 1024 -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json )      

SET ( TEST_NUMBER "012" )
SET ( SUBTEST_NUMBER "a" )
cet_test(JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t HANDBUILT  TEST_EXEC JSONOverlay_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json 
  TEST_ARGS -f -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json )

SET ( SUBTEST_NUMBER "b" )
cet_test(JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t HANDBUILT  TEST_EXEC JSONOverlay_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json 
  TEST_ARGS -m 2 -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json )        

SET ( TEST_NUMBER "013" )
SET ( SUBTEST_NUMBER "a" )
cet_test(JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t HANDBUILT  TEST_EXEC JSONOverlay_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json 
  TEST_ARGS -f -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json )

SET ( SUBTEST_NUMBER "b" )
cet_test(JSONOverlay_${TEST_NAME}_${TEST_NUMBER}${SUBTEST_NUMBER}_t HANDBUILT  TEST_EXEC JSONOverlay_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.cmp.json
    ${CMAKE_CURRENT_SOURCE_DIR}/test${TEST_NUMBER}.src.json 
  TEST_ARGS -m 1 -s test${TEST_NUMBER}.src.json -x test${TEST_NUMBER}.cmp.json )    
