art_make_exec(tojson
  LIBRARIES
  artdaq-database_FhiclJson
  ${FHICLCPP}  
  ${CETLIB}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
) 

art_make_exec(tofhicl
  LIBRARIES
  artdaq-database_FhiclJson
  ${FHICLCPP}  
  ${CETLIB}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
) 

art_make_exec(readfhicl
  LIBRARIES
  artdaq-database_FhiclJson
  ${FHICLCPP}  
  ${CETLIB}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
)

art_make_exec(readjson
  LIBRARIES
  artdaq-database_FhiclJson
  ${FHICLCPP}  
  ${CETLIB}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
) 

art_make_exec(convertjson
  LIBRARIES
  artdaq-database_FhiclJson
  ${FHICLCPP}  
  ${CETLIB}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
) 

find_ups_product( mongodb ${mongodb_product_version} )

include_directories($ENV{MONGODB_FQ_DIR}/include/bsoncxx/v0.3)
include_directories($ENV{MONGODB_FQ_DIR}/include/mongocxx/v0.3)

find_library(BSONCXXLIB NAMES bsoncxx HINTS $ENV{MONGODB_FQ_DIR}/lib )
find_library(MONGOCXXLIB NAMES mongocxx HINTS $ENV{MONGODB_FQ_DIR}/lib )

 art_make_exec(conftool
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
  ${Boost_PROGRAM_OPTIONS_LIBRARY}  
)

 art_make_exec(conftool2
  LIBRARIES
  artdaq-database_StackTrace  
  artdaq-database_BasicTypes
  artdaq-database_FhiclJson
  artdaq-database_JsonDocument
  artdaq-database_ConfigurationDB
  artdaq-database_StorageProviders_MongoDB
  artdaq-database_StorageProviders_FileSystemDB
  artdaq-database_NodeJSAddons_conftool
  ${FHICLCPP}
  ${CETLIB}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}  
)

install_headers()
install_source()
