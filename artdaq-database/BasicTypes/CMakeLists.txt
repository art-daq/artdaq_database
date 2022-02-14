
  if(USE_FHICLCPP)
  cet_make(
  LIBRARIES
  ${FHICLCPP_DATAFORMAT_LIB}
  artdaq-database_DataFormats_Json
  artdaq-database_DataFormats_Xml
  cetlib_except
  )
else()
cet_make(
	EXCLUDE data_fhicl.cpp
  LIBRARIES
  artdaq-database_DataFormats_Json
  artdaq-database_DataFormats_Xml
  cetlib_except
  )

  endif()

install_headers()
install_source()
