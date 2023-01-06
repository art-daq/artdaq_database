cet_make_exec(NAME MongoXml_t
  LIBRARIES
    
  artdaq-database_BasicTypes
  artdaq-database_DataFormats_Json
  artdaq-database_DataFormats_Xml
  artdaq-database_JsonDocument
  artdaq-database_StorageProviders_MongoDB
  #fhiclcpp
  #${SQLITE3}
  cetlib
  cetlib_except
  Boost::program_options
) 

add_subdirectory(test001)
add_subdirectory(test002)
add_subdirectory(test003)
add_subdirectory(test004)

install_source()
