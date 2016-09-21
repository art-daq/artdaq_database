cet_make_exec(CONFConvert_t
  LIBRARIES
  artdaq-database_StackTrace
  artdaq-database_DataFormats_common
  artdaq-database_DataFormats_Json
  artdaq-database_DataFormats_Conf
  ${CETLIB}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
) 

add_subdirectory(test001)
add_subdirectory(test002)

install_source()
