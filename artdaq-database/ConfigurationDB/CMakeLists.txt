find_ups_product( mongodb ${mongodb_product_version} )

include_directories($ENV{MONGODB_FQ_DIR}/include/bsoncxx/v0.3)
include_directories($ENV{MONGODB_FQ_DIR}/include/mongocxx/v0.3)

#find_library(BSONCXXLIB NAMES bsoncxx HINTS $ENV{MONGODB_FQ_DIR}/lib )
#find_library(MONGOCXXLIB NAMES mongocxx HINTS $ENV{MONGODB_FQ_DIR}/lib )

art_make(
  LIB_LIBRARIES
  artdaq-database_FhiclJson
  artdaq-database_JsonDocument
  artdaq-database_BasicTypes
  artdaq-database_StorageProviders_MongoDB  
  artdaq-database_StorageProviders_FileSystemDB  
 # ${BSONCXXLIB}
 # ${MONGOCXXLIB}  
  ${FHICLCPP}  
  ${CETLIB}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}    
  )

install_headers()
install_source()
