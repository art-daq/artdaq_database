
find_ups_product( mongodb ${mongodb_product_version} )

include_directories($ENV{MONGODB_INC})

 

find_library(BSONCXXLIB NAMES bsoncxx HINTS $ENV{MONGODB_LIB} )
find_library(MONGOCXXLIB NAMES mongocxx HINTS $ENV{MONGODB_LIB} )

 art_make_exec(conftool
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


art_make_exec(readfhicl
  LIBRARIES
  artdaq-database_DataFormats_common
  artdaq-database_DataFormats_Fhicl
  ${FHICLCPP}
  ${SQLITE3}  
  ${CETLIB}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
)


install_headers()
install_source()

