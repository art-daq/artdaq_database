cet_make(
  LIBRARIES
  artdaq-database_SharedCommon
  artdaq-database_DataFormats_Json
  ${FHICLCPP}
  ${CETLIB}
  ${CETLIB_EXCEPT}
  )
  if(FHICLCPP_IS_STATIC)
  add_dependencies(artdaq-database_SharedCommon ${FHICLCPP_TARGET})
  endif()

install_headers()
install_source()
