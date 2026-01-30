SET ( TEST_NAME "OtsDaqCPPInterface" )
SET ( TEST_NUMBER "026" )

add_executable(StringLineRoundTrip_t StringLineRoundTrip_t.cc)

target_link_libraries(StringLineRoundTrip_t PRIVATE
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

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test_cases.txt
               ${CMAKE_CURRENT_BINARY_DIR}/test_cases.txt COPYONLY)

SET ( TEST_DBPROVIDER "filesystem" )
add_test(NAME ${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_t
         COMMAND StringLineRoundTrip_t
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}_otsdb026;QUICK_TEST=1")

SET ( TEST_DBPROVIDER "mongo" )
add_test(NAME ${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_t
         COMMAND StringLineRoundTrip_t
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_MONGO}_otsdb026;QUICK_TEST=1")

install_headers()
install_source()
