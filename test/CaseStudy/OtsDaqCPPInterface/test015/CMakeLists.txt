SET ( TEST_NAME "OtsDaqCPPInterface" )
SET ( TEST_NUMBER "015" )

add_executable(CompositionNaming_t CompositionNaming_t.cc)
target_link_libraries(CompositionNaming_t PRIVATE
  test_CaseStudy_OtsDaqCPPInterface
  artdaq-database::BasicTypes
  artdaq-database::ConfigurationDB
  Boost::unit_test_framework
)

add_test(NAME ${TEST_NAME}_filesystem_${TEST_NUMBER}_t
         COMMAND CompositionNaming_t)
set_tests_properties(${TEST_NAME}_filesystem_${TEST_NUMBER}_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}_otsdb015")

add_test(NAME ${TEST_NAME}_mongo_${TEST_NUMBER}_t
         COMMAND CompositionNaming_t)
set_tests_properties(${TEST_NAME}_mongo_${TEST_NUMBER}_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_MONGO}_otsdb015")
