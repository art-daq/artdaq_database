cet_make_exec(NAME CONFConvert_t
  LIBRARIES
  
  artdaq-database_DataFormats_Json
  artdaq-database_DataFormats_Conf
  cetlib
) 

add_subdirectory(test001)
add_subdirectory(test002)

install_source()
