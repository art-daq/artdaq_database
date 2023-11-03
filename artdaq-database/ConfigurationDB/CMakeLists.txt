cet_make(
  LIBRARIES
  ${FHICLCPP_DATAFORMAT_LIB}
  artdaq-database_DataFormats_Json
  artdaq-database_DataFormats_Xml  
  artdaq-database_JsonDocument
  artdaq-database_BasicTypes
  artdaq-database_StorageProviders_MongoDB  
  artdaq-database_StorageProviders_FileSystemDB
  artdaq-database_StorageProviders_UconDB
  Boost::program_options
  cetlib_except
  -lpthread
  )

  if(USE_FHICLCPP)
  target_compile_definitions(artdaq-database_ConfigurationDB PUBLIC USE_FHICLCPP)
  endif()

install_headers()
install_source()
