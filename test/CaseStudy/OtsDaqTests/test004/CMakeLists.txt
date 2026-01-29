add_executable(OtsDaqTests_ThreeVersionsBug_t ThreeVersionsBug_t.cc)
target_link_libraries(OtsDaqTests_ThreeVersionsBug_t
  test_CaseStudy_OtsDaqTests
  artdaq-database::ConfigurationDB
  artdaq-database::BasicTypes
  Boost::unit_test_framework
)

add_test(NAME OtsDaqTests_ThreeVersionsBug_filesystem_t
         COMMAND OtsDaqTests_ThreeVersionsBug_t
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(OtsDaqTests_ThreeVersionsBug_filesystem_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}_threeversions")

add_test(NAME OtsDaqTests_ThreeVersionsBug_mongo_t
         COMMAND OtsDaqTests_ThreeVersionsBug_t
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(OtsDaqTests_ThreeVersionsBug_mongo_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_MONGO}_threeversions")
