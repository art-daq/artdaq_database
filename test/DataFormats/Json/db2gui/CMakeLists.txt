cet_make_exec(ConvertJson_t
  LIBRARIES
  
  artdaq-database_DataFormats_Json
  fhiclcpp
  cetlib
  cetlib_except
  #${SQLITE3}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
) 

add_subdirectory(test001)
add_subdirectory(test002)
add_subdirectory(test003)
add_subdirectory(test004)

install_source()
