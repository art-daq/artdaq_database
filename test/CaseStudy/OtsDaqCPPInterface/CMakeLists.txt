art_make(
  LIBRARY_NAME artdaq-database_OtsDaqCPPInterface
  LIB_LIBRARIES
  artdaq-database_ConfigurationDB
  )

add_subdirectory(test001)
  
install_headers()
install_source()

