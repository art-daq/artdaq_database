add_executable(OtsDaqTests_DatabaseConfiguration_t DatabaseConfiguration_t.cc)
target_link_libraries(OtsDaqTests_DatabaseConfiguration_t
  test_CaseStudy_OtsDaqTests
  artdaq-database::ConfigurationDB
  Boost::program_options
  Boost::unit_test_framework
)

add_test(NAME OtsDaqTests_filesystem_t
         COMMAND OtsDaqTests_DatabaseConfiguration_t
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(OtsDaqTests_filesystem_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_FILESYSTEM}f")

add_test(NAME OtsDaqTests_mongo_t
         COMMAND OtsDaqTests_DatabaseConfiguration_t
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(OtsDaqTests_mongo_t PROPERTIES
    ENVIRONMENT "ARTDAQ_DATABASE_URI=${ARTDAQ_DATABASE_URI_MONGO}f")

install_headers()
install_source()
