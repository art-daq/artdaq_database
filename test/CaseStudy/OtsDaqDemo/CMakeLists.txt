find_ups_product( mongodb ${mongodb_product_version} )

include_directories($ENV{MONGODB_INC})

 

find_library(BSONCXXLIB NAMES bsoncxx HINTS $ENV{MONGODB_LIB} )
find_library(MONGOCXXLIB NAMES mongocxx HINTS $ENV{MONGODB_LIB} )

 art_make_exec(OtsDaqDemo_t
  LIBRARIES
  artdaq-database_StackTrace  
  artdaq-database_BasicTypes
  artdaq-database_FhiclJson
  artdaq-database_JsonDocument
  artdaq-database_ConfigurationDB
  artdaq-database_StorageProviders_MongoDB
  artdaq-database_StorageProviders_FileSystemDB
  ${FHICLCPP}
  ${SQLITE3}
  ${CETLIB}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}  
) 


#add_subdirectory(test001)
#add_subdirectory(test002)
#add_subdirectory(test004)
add_subdirectory(test005)
install_source()
