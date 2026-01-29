cet_make_library(
	SOURCE
	confjsondb.cpp
	conf_reader.cpp
	conf_writer.cpp
	convertconf2json.cpp
  LIBRARIES
  artdaq-database::SharedCommon
  artdaq-database::DataFormats_Json
  )

install_headers()
install_source()
