art_make_exec(tojson
  LIBRARIES
  artdaq-database_FhiclJson
  ${FHICLCPP}  
  ${CETLIB}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
) 

art_make_exec(tofhicl
  LIBRARIES
  artdaq-database_FhiclJson
  ${FHICLCPP}  
  ${CETLIB}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
) 

install_headers()
install_source()
