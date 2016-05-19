
art_make_exec(readfhicl
  LIBRARIES
  artdaq-database_DataFormats_common
  artdaq-database_DataFormats_Fhicl
  ${FHICLCPP}  
  ${CETLIB}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
)


install_headers()
install_source()
