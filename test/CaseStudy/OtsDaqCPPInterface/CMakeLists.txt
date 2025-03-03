cet_make_library( 
SOURCE
ConfigurationBaseImpl.cpp
DatabaseConfigurationInterface.cpp
  LIBRARIES
  artdaq-database_ConfigurationDB
  cetlib_except
  Boost::unit_test_framework
  )

add_subdirectory(test001)
  
install_headers()
install_source()
