SET ( TEST_NAME "OtsDaqCPPInterface" )
SET ( TEST_NUMBER "013" )

add_executable(DataIntegrity_t DataIntegrity_t.cc)
target_link_libraries(DataIntegrity_t PRIVATE
  test_CaseStudy_OtsDaqCPPInterface
  artdaq-database::BasicTypes
  artdaq-database::ConfigurationDB
  Boost::unit_test_framework
)

add_test(NAME ${TEST_NAME}_filesystem_${TEST_NUMBER}_t
         COMMAND DataIntegrity_t)
set_tests_properties(${TEST_NAME}_filesystem_${TEST_NUMBER}_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}_otsdb013")

add_test(NAME ${TEST_NAME}_mongo_${TEST_NUMBER}_t
         COMMAND DataIntegrity_t)
set_tests_properties(${TEST_NAME}_mongo_${TEST_NUMBER}_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_MONGO}_otsdb013")
