art_make_exec(readjson_t
  LIBRARIES
  artdaq-database_StackTrace
  artdaq-database_FhiclJson
  ${CETLIB}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
) 

add_subdirectory(test001)

install_source()
