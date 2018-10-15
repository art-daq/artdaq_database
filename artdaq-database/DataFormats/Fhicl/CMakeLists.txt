#cet_add_compiler_flags(-fvisibility=hidden)
cet_make(
  LIBRARIES
  artdaq-database_SharedCommon
  artdaq-database_DataFormats_Json
  ${FHICLCPP}
  )

install_headers()
install_source()
