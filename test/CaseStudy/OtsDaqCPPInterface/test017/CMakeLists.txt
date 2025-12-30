SET ( TEST_NAME "OtsDaqCPPInterface" )
SET ( TEST_NUMBER "017" )

add_executable(WorkflowIntegration_t WorkflowIntegration_t.cc)
target_link_libraries(WorkflowIntegration_t PRIVATE
  test_CaseStudy_OtsDaqCPPInterface
  artdaq-database_BasicTypes
  artdaq-database_ConfigurationDB
  Boost::unit_test_framework
)

add_test(NAME ${TEST_NAME}_filesystem_${TEST_NUMBER}_t
         COMMAND WorkflowIntegration_t)
set_tests_properties(${TEST_NAME}_filesystem_${TEST_NUMBER}_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}_otsdb017")

add_test(NAME ${TEST_NAME}_mongo_${TEST_NUMBER}_t
         COMMAND WorkflowIntegration_t)
set_tests_properties(${TEST_NAME}_mongo_${TEST_NUMBER}_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_MONGO}_otsdb017")
