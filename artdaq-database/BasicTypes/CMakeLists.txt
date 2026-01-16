
  cet_make_library(
    SOURCE
      base64.cpp
      data_fhicl.cpp
      data_json.cpp
      data_xml.cpp
    LIBRARIES
      artdaq-database::DataFormats_Json
      artdaq-database::DataFormats_Xml
      artdaq-database::DataFormats_Fhicl
  )

install_headers()
install_source()
