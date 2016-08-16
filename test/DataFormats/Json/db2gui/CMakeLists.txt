art_make_exec(ConvertJson_t
  LIBRARIES
  artdaq-database_StackTrace
  artdaq-database_DataFormats_common
  artdaq-database_DataFormats_Json
  ${CETLIB}
  ${FHICLCPP}
  ${SQLITE3}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
) 

add_subdirectory(test001)
add_subdirectory(test002)

add_subdirectory(test003)

add_subdirectory(test004)

install_source()
