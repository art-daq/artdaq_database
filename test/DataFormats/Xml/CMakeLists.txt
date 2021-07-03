cet_make_exec(XMLConvert_t
  LIBRARIES
  
  artdaq-database_DataFormats_Json
  artdaq-database_DataFormats_Xml
  ${CETLIB}
  ${CETLIB_EXCEPT}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
) 

add_subdirectory(test001)
add_subdirectory(test002)

install_source()
