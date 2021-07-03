cet_make_exec(readjson_t
  LIBRARIES
  
  artdaq-database_DataFormats_Json
  ${FHICLCPP}
  ${CETLIB}
  ${CETLIB_EXCEPT}
  #${SQLITE3}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
) 

add_subdirectory(test001)

install_source()
