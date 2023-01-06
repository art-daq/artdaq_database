cet_make_exec(NAME XMLConvert_t
  LIBRARIES
  
  artdaq-database_DataFormats_Json
  artdaq-database_DataFormats_Xml
  cetlib
  cetlib_except
  Boost::program_options
) 

add_subdirectory(test001)
add_subdirectory(test002)

install_source()
