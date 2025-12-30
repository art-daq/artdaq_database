SET ( TEST_NAME "OtsDaqCPPInterface" )
SET ( TEST_NUMBER "012" )

add_executable(VersionEdgeCases_t VersionEdgeCases_t.cc)
target_link_libraries(VersionEdgeCases_t PRIVATE
  test_CaseStudy_OtsDaqCPPInterface
  artdaq-database_BasicTypes
  artdaq-database_ConfigurationDB
  Boost::unit_test_framework
)

add_test(NAME ${TEST_NAME}_filesystem_${TEST_NUMBER}_t
         COMMAND VersionEdgeCases_t)
set_tests_properties(${TEST_NAME}_filesystem_${TEST_NUMBER}_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}_otsdb012")

add_test(NAME ${TEST_NAME}_mongo_${TEST_NUMBER}_t
         COMMAND VersionEdgeCases_t)
set_tests_properties(${TEST_NAME}_mongo_${TEST_NUMBER}_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_MONGO}_otsdb012")
