SET ( TEST_NAME "OtsDaqCPPInterface" )
SET ( TEST_NUMBER "021" )

add_executable(MarkReadonlyExpanded_t MarkReadonlyExpanded_t.cc)

target_link_libraries(MarkReadonlyExpanded_t PRIVATE
  test_CaseStudy_OtsDaqCPPInterface
  artdaq-database::BasicTypes
  artdaq-database::DataFormats_Fhicl
  artdaq-database::DataFormats_Json
  artdaq-database::JsonDocument
  artdaq-database::ConfigurationDB
  artdaq-database::StorageProviders_MongoDB
  artdaq-database::StorageProviders_FileSystemDB
  artdaq-database::StorageProviders_UconDB
  Boost::program_options
  Boost::unit_test_framework
)

SET ( TEST_DBPROVIDER "filesystem" )
add_test(NAME ${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_t
         COMMAND MarkReadonlyExpanded_t
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}_otsdb021")

SET ( TEST_DBPROVIDER "mongo" )
add_test(NAME ${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_t
         COMMAND MarkReadonlyExpanded_t
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(${TEST_NAME}_${TEST_DBPROVIDER}_${TEST_NUMBER}_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_MONGO}_otsdb021")

install_headers()
install_source()
