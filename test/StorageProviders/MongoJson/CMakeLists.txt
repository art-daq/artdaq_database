cet_make_exec(MongoJson_t
  LIBRARIES
  artdaq-database_SharedCommon
  artdaq-database_BasicTypes
  artdaq-database_DataFormats_Json
  artdaq-database_JsonDocument
  artdaq-database_StorageProviders_MongoDB
  ${Boost_PROGRAM_OPTIONS_LIBRARY}  
) 

add_subdirectory(test001)
#add_subdirectory(test002)
#add_subdirectory(test003)
#add_subdirectory(test004)
add_subdirectory(test005)

install_source()
