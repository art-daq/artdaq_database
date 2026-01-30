add_library(test_CaseStudy_OtsDaqTests
  ConfigurationBaseImpl.cpp
  DatabaseConfigurationInterface.cpp
)

target_link_libraries(test_CaseStudy_OtsDaqTests PUBLIC
  artdaq-database::ConfigurationDB
  Boost::unit_test_framework
)

add_subdirectory(test001)
add_subdirectory(test002)
add_subdirectory(test003)
add_subdirectory(test004)
add_subdirectory(test005)

install_headers()
install_source()
