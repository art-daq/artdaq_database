cet_make_exec(NAME readjson_t
  LIBRARIES
  
  artdaq-database_DataFormats_Json
  fhiclcpp
  cetlib
  cetlib_except
  #${SQLITE3}
  Boost::program_options
) 

add_subdirectory(test001)

install_source()
