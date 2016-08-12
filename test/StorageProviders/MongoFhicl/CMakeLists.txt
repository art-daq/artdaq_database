find_ups_product( mongodb ${mongodb_product_version} )

include_directories($ENV{MONGODB_INC})

 

find_library(BSONCXXLIB NAMES bsoncxx HINTS $ENV{MONGODB_LIB} )
find_library(MONGOCXXLIB NAMES mongocxx HINTS $ENV{MONGODB_LIB} )

 art_make_exec(MongoFhicl_t
  LIBRARIES
  artdaq-database_StackTrace  
  artdaq-database_BasicTypes
  artdaq-database_DataFormats_common
  artdaq-database_DataFormats_Fhicl
  artdaq-database_DataFormats_Json
  artdaq-database_JsonDocument
  artdaq-database_StorageProviders_MongoDB
  ${FHICLCPP}
  ${SQLITE3}
  ${CETLIB}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}    
) 

add_subdirectory(test001)
add_subdirectory(test002)
add_subdirectory(test003)
add_subdirectory(test004)

install_source()
