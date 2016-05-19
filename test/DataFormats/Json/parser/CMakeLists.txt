art_make_exec(readjson_t
  LIBRARIES
  artdaq-database_StackTrace
  artdaq-database_DataFormats_common
  artdaq-database_DataFormats_Json
  ${CETLIB}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
) 

add_subdirectory(test001)

install_source()
