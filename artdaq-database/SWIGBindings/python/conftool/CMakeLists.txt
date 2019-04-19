create_python_addon ( ADDON_NAME "conftoolp"
					  LIBRARIES
					  artdaq-database_ConfigurationDB
					  INCLUDES
					  ${CMAKE_SOURCE_DIR}/artdaq-database)

install_headers()
install_source()
