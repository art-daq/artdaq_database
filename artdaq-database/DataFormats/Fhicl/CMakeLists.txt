cet_make_library(
	SOURCE
	convertfhicl2jsondb.cpp
	fhicljsondb.cpp
	fhicl_reader.cpp
	fhicl_types.cpp
	fhicl_writer.cpp
	helper_functions.cpp
  LIBRARIES PUBLIC
  artdaq-database::SharedCommon
  artdaq-database::DataFormats_Json
  fhiclcpp::fhiclcpp
  )

install_headers()
install_source()
