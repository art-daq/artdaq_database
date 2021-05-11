cet_make(
  LIBRARIES
  artdaq-database_SharedCommon
  artdaq-database_DataFormats_Json
  $<TARGET_FILE:${FHICLCPP}>
  $<TARGET_FILE:${CETLIB}>
  $<TARGET_FILE:${CETLIB_EXCEPT}>
  )
  add_dependencies(artdaq-database_DataFormats_Fhicl ${FHICLCPP})

install_headers()
install_source()
