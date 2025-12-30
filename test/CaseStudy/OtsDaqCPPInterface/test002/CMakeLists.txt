SET ( TEST_NAME "DuplicateDetection" )
SET ( TEST_NUMBER "002" )

add_executable(DuplicateDetection_t DuplicateDetection_t.cc)

target_link_libraries(DuplicateDetection_t PRIVATE
  test_CaseStudy_OtsDaqCPPInterface
  artdaq-database_BasicTypes
  artdaq-database_DataFormats_Fhicl
  artdaq-database_DataFormats_Json
  artdaq-database_JsonDocument
  artdaq-database_ConfigurationDB
  artdaq-database_StorageProviders_MongoDB
  artdaq-database_StorageProviders_FileSystemDB
  artdaq-database_StorageProviders_UconDB
  Boost::program_options
  Boost::unit_test_framework
)

SET ( TEST_DBPROVIDER "filesystem" )
# Register test with CTest - use unique database path for isolation
add_test(NAME ${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_t
         COMMAND DuplicateDetection_t
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}_otsdb002")

SET ( TEST_DBPROVIDER "mongo" )
# Register test with CTest - use unique database path for isolation
add_test(NAME ${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_t
         COMMAND DuplicateDetection_t
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_MONGO}_otsdb002")

install_headers()
install_source()
