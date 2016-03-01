cet_test(readjson_01_t HANDBUILT  TEST_EXEC readjson_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test001.json
  TEST_ARGS -c test001.json)

cet_test(readjson_02_t HANDBUILT  TEST_EXEC readjson_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test002.json
  TEST_ARGS -c test002.json)


cet_test(readjson_03_t HANDBUILT  TEST_EXEC readjson_t 
  DATAFILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/test003.json
  TEST_ARGS -c test003.json)
  