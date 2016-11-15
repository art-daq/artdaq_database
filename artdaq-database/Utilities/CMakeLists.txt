cet_make_exec(conftool
  LIBRARIES
  artdaq-database_StackTrace  
  artdaq-database_BasicTypes
  artdaq-database_DataFormats_common
  artdaq-database_DataFormats_Fhicl
  artdaq-database_DataFormats_Json
  artdaq-database_JsonDocument
  artdaq-database_ConfigurationDB
  artdaq-database_StorageProviders_MongoDB
  artdaq-database_StorageProviders_FileSystemDB
  ${FHICLCPP}
  ${SQLITE3}
  ${CETLIB}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}  
) 


cet_make_exec(readfhicl
  LIBRARIES
  artdaq-database_DataFormats_common
  artdaq-database_DataFormats_Fhicl
  ${FHICLCPP}
  ${SQLITE3}  
  ${CETLIB}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
)

cet_script(conftool.sh)

install_headers()
install_source()

