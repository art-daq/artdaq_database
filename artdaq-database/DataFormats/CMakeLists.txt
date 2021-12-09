add_subdirectory(Json)
if(USE_FHICLCPP)
add_subdirectory(Fhicl)
endif()
add_subdirectory(Xml)
#add_subdirectory(Conf)

install_headers()
install_source()
