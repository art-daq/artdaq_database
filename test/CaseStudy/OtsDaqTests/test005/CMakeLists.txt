# Build executable
add_executable(OtsDaqTests_CaseSensitivity_t CaseSensitivity_t.cc)
target_link_libraries(OtsDaqTests_CaseSensitivity_t
  test_CaseStudy_OtsDaqTests
  artdaq-database_ConfigurationDB
  artdaq-database_BasicTypes
  Boost::unit_test_framework
)

add_test(NAME OtsDaqTests_CaseSensitivity_filesystem_t
         COMMAND OtsDaqTests_CaseSensitivity_t
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(OtsDaqTests_CaseSensitivity_filesystem_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}_casesensitivity")

add_test(NAME OtsDaqTests_CaseSensitivity_mongo_t
         COMMAND OtsDaqTests_CaseSensitivity_t
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(OtsDaqTests_CaseSensitivity_mongo_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_MONGO}_casesensitivity")
