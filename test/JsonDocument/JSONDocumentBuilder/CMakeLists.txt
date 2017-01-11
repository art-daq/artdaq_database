cet_make_exec(JSONDocumentBuilder_t
  LIBRARIES
  
  artdaq-database_JsonDocument
  ${BSONCXXLIB}
  ${CETLIB}
  ${FHICLCPP} 
  ${SQLITE3}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
) 

#build document tests
#add_subdirectory(test001)
#add_subdirectory(test002)
#add_subdirectory(test003)

#alias add remove tests
#add_subdirectory(test004)
#add_subdirectory(test005)

#add to global configuration
add_subdirectory(test006)

#add to global configuration
#add_subdirectory(test007)

#mark readonly and deleted
#add_subdirectory(test008)
#add_subdirectory(test009)

install_source()
