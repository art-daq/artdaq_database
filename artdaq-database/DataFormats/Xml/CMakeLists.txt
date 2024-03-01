cet_make_library(
  SOURCE
  convertxml2json.cpp
  xmljsondb.cpp
  xml_reader.cpp
  xml_writer.cpp
  LIBRARIES
  artdaq-database_SharedCommon
  artdaq-database_DataFormats_Json
  cetlib_except
  )
  
install_headers()
install_source()
