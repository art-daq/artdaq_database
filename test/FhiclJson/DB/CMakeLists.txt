art_make_exec(FhiclJsonDB_t
  LIBRARIES
  artdaq-database_StackTrace
  artdaq-database_FhiclJson
  ${CETLIB}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
) 

add_subdirectory(test001)
add_subdirectory(test002)

add_subdirectory(test003)
add_subdirectory(test004)

install_source()
