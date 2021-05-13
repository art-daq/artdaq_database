cet_make(
  LIBRARIES
  artdaq-database_SharedCommon
  artdaq-database_DataFormats_Json
  ${FHICLCPP}
  ${CETLIB}
  ${CETLIB_EXCEPT}
  )

install_headers()
install_source()
