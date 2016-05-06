art_make_exec(XMLConvert_t
  LIBRARIES
  artdaq-database_StackTrace
  artdaq-database_FhiclJson
  artdaq-database_XmlJson
  
  ${CETLIB}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
) 

add_subdirectory(test001)
add_subdirectory(test002)

install_source()
