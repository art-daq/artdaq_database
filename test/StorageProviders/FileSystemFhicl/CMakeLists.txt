find_ups_product( mongodb ${mongodb_product_version} )

include_directories($ENV{MONGODB_FQ_DIR}/include/bsoncxx/v0.3)
include_directories($ENV{MONGODB_FQ_DIR}/include/mongocxx/v0.3)

find_library(BSONCXXLIB NAMES bsoncxx HINTS $ENV{MONGODB_FQ_DIR}/lib )
find_library(MONGOCXXLIB NAMES mongocxx HINTS $ENV{MONGODB_FQ_DIR}/lib )

 art_make_exec(FileSystemFhicl_t
  LIBRARIES
  artdaq-database_StackTrace  
  artdaq-database_BasicTypes
  artdaq-database_FhiclJson
  artdaq-database_JsonDocument
  artdaq-database_StorageProviders_FileSystemDB
  ${FHICLCPP}
  ${CETLIB}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}    
) 

add_subdirectory(test001)
#add_subdirectory(test002)
add_subdirectory(test003)
add_subdirectory(test004)

install_source()
