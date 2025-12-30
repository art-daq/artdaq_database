# ValidateJson test - reads JSON file, parses to AST, serializes, parses again, compares ASTs
add_executable(ValidateJson_t ValidateJson_t.cc)

target_link_libraries(ValidateJson_t PRIVATE
  artdaq-database_BasicTypes
  artdaq-database_DataFormats_Fhicl
  artdaq-database_DataFormats_Json
  artdaq-database_JsonDocument
  artdaq-database_ConfigurationDB
  artdaq-database_StorageProviders_MongoDB
  artdaq-database_StorageProviders_FileSystemDB
  artdaq-database_StorageProviders_UconDB
  Boost::program_options
)

SET ( TEST_NAME "ValidateJson" )

# Register ValidateJson tests with CTest
add_test(NAME ${TEST_NAME}_simple_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/ValidateJson_t ${CMAKE_CURRENT_SOURCE_DIR}/test_simple.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_test(NAME ${TEST_NAME}_nested_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/ValidateJson_t ${CMAKE_CURRENT_SOURCE_DIR}/test_nested.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_test(NAME ${TEST_NAME}_arrays_t
         COMMAND ${TOP_CMAKE_BINARY_DIR}/bin/ValidateJson_t ${CMAKE_CURRENT_SOURCE_DIR}/test_arrays.json
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
