SET ( TEST_NUMBER "001" )
SET ( TEST_NAME "jsonconvert" )

cet_test(JSONConvert_01_t HANDBUILT  TEST_EXEC JSONConvert_t.cc 
  DATAFILES 
  TEST_ARGS )


