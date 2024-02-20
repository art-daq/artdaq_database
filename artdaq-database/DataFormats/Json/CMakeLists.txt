cet_make_library(
  SOURCE
  convertjson2guijson.cpp
  json_reader.cpp
  json_types.cpp
  json_writer.cpp
  LIBRARIES
  artdaq-database_SharedCommon
  cetlib_except
  )

install_headers()
install_source()
