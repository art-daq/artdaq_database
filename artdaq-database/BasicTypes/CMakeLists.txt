
if(USE_FHICLCPP)
  cet_make_library(
    SOURCE
      base64.cpp
      data_fhicl.cpp
      data_json.cpp
      data_xml.cpp
    LIBRARIES
      ${FHICLCPP_DATAFORMAT_LIB}
      artdaq-database_DataFormats_Json
      artdaq-database_DataFormats_Xml
      cetlib_except
  )

else()
  cet_make_library(
    SOURCE
      base64.cpp
      data_json.cpp
      data_xml.cpp
    LIBRARIES
      artdaq-database_DataFormats_Json
      artdaq-database_DataFormats_Xml
      cetlib_except
  )

endif()

install_headers()
install_source()
