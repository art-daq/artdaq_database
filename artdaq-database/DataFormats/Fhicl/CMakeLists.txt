cet_make_library(
	SOURCE
	convertfhicl2jsondb.cpp
	fhicljsondb.cpp
	fhicl_reader.cpp
	fhicl_types.cpp
	fhicl_writer.cpp
	helper_functions.cpp
  LIBRARIES
  artdaq-database_SharedCommon
  artdaq-database_DataFormats_Json
  fhiclcpp
  cetlib
  cetlib_except
  )
if(FHICLCPP_IS_STATIC)
  add_dependencies(artdaq-database_DataFormats_Fhicl ${FHICLCPP_TARGET})
endif()

install_headers()
install_source()
