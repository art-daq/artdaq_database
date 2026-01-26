add_library(test_CaseStudy_OtsDaqCPPInterface
  ConfigurationBaseImpl.cpp
  DatabaseConfigurationInterface.cpp
)

target_link_libraries(test_CaseStudy_OtsDaqCPPInterface PUBLIC
  artdaq-database_ConfigurationDB
  Boost::unit_test_framework
)

add_subdirectory(test001)
add_subdirectory(test002)
add_subdirectory(test003)
add_subdirectory(test004)
add_subdirectory(test005)
add_subdirectory(test006)
add_subdirectory(test007)
add_subdirectory(test008)
add_subdirectory(test009)
add_subdirectory(test010)
add_subdirectory(test011)
add_subdirectory(test012)
add_subdirectory(test013)
add_subdirectory(test014)
add_subdirectory(test015)
add_subdirectory(test016)
add_subdirectory(test017)
add_subdirectory(test018)
add_subdirectory(test019)
add_subdirectory(test020)
add_subdirectory(test021)
add_subdirectory(test022)
add_subdirectory(test023)
add_subdirectory(test024)
add_subdirectory(test025)
add_subdirectory(test026)
install_headers()
install_source()
