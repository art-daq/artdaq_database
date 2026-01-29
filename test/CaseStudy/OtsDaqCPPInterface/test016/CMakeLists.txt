SET ( TEST_NAME "OtsDaqCPPInterface" )
SET ( TEST_NUMBER "016" )

add_executable(SearchPatterns_t SearchPatterns_t.cc)
target_link_libraries(SearchPatterns_t PRIVATE
  test_CaseStudy_OtsDaqCPPInterface
  artdaq-database::BasicTypes
  artdaq-database::ConfigurationDB
  Boost::unit_test_framework
)

add_test(NAME ${TEST_NAME}_filesystem_${TEST_NUMBER}_t
         COMMAND SearchPatterns_t)
set_tests_properties(${TEST_NAME}_filesystem_${TEST_NUMBER}_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}_otsdb016")

add_test(NAME ${TEST_NAME}_mongo_${TEST_NUMBER}_t
         COMMAND SearchPatterns_t)
set_tests_properties(${TEST_NAME}_mongo_${TEST_NUMBER}_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_MONGO}_otsdb016")
