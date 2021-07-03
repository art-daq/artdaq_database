cet_make_exec(ConvertFhicl_t
  LIBRARIES
  
  artdaq-database_DataFormats_Json
  artdaq-database_DataFormats_Fhicl
  ${FHICLCPP}
  ${CETLIB}
  ${CETLIB_EXCEPT}
  #${SQLITE3}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
) 

add_subdirectory(test001)
add_subdirectory(test002)

add_subdirectory(test003)
add_subdirectory(test004)
add_subdirectory(test005)

add_subdirectory(test006)
add_subdirectory(test007)

install_source()
