cet_make_exec(readjson_t
  LIBRARIES
  
  artdaq-database_DataFormats_Json
  fhiclcpp
  cetlib
  cetlib_except
  #${SQLITE3}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
) 

add_subdirectory(test001)

install_source()
