cet_make(
  LIBRARY_NAME artdaq-database_OtsDaqCPPInterface
  LIBRARIES
  artdaq-database_ConfigurationDB
  ${CETLIB_EXCEPT}
  )

add_subdirectory(test001)
  
install_headers()
install_source()

