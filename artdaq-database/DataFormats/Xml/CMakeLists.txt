cet_make_library(
  SOURCE
  convertxml2json.cpp
  xmljsondb.cpp
  xml_reader.cpp
  xml_writer.cpp
  LIBRARIES
  artdaq-database::SharedCommon
  artdaq-database::DataFormats_Json
  )

install_headers()
install_source()
