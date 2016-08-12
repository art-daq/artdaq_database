find_ups_product( mongodb ${mongodb_product_version} )

include_directories($ENV{MONGODB_INC})

 

find_library(BSONCXXLIB NAMES bsoncxx HINTS $ENV{MONGODB_LIB} )
find_library(MONGOCXXLIB NAMES mongocxx HINTS $ENV{MONGODB_LIB} )

 art_make_exec(JSONConvert_t
  LIBRARIES
  artdaq-database_StackTrace  
  artdaq-database_BasicTypes
  artdaq-database_FhiclJson
  artdaq-database_JsonDocument
  artdaq-database_ConfigurationDB
  artdaq-database_StorageProviders_MongoDB
  artdaq-database_StorageProviders_FileSystemDB
  ${FHICLCPP}
  ${CETLIB}
  ${SQLITE3}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}  
) 


add_subdirectory(test001)
install_source()
