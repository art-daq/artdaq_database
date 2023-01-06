cet_make_exec(NAME JSONDocument_t
  LIBRARIES
  
  artdaq-database_JsonDocument
  fhiclcpp
  cetlib
  cetlib_except
  #${SQLITE3}
  Boost::program_options
) 

#replace tests
add_subdirectory(test001)
add_subdirectory(test002)
add_subdirectory(test003)
add_subdirectory(test004)

#insert tests
add_subdirectory(test101)
add_subdirectory(test102)
add_subdirectory(test103)
add_subdirectory(test104)

#delete tests
add_subdirectory(test201)

#collection tests
add_subdirectory(test301)
add_subdirectory(test302)

install_source()
