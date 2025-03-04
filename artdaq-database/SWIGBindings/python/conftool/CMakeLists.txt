

if(USE_FHICLCPP)
  set_property(SOURCE conftool.i PROPERTY COMPILE_DEFINITIONS ADFHICLCPP)
endif()

create_python_addon ( ADDON_NAME "conftoolp"
					  LIBRARIES
					  artdaq-database::ConfigurationDB)

install_headers()
install_source()
