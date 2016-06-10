find_ups_product( mongodb ${mongodb_product_version} )

include_directories($ENV{MONGODB_FQ_DIR}/include/bsoncxx/v0.3)
include_directories($ENV{MONGODB_FQ_DIR}/include/mongocxx/v0.3)

find_library(BSONCXXLIB NAMES bsoncxx HINTS $ENV{MONGODB_FQ_DIR}/lib )
find_library(MONGOCXXLIB NAMES mongocxx HINTS $ENV{MONGODB_FQ_DIR}/lib )

 art_make_exec(ValidateJson_t
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
  ${CETLIB}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}  
)

SET ( TEST_NAME "ValidateJson" )

cet_test(${TEST_NAME}_t HANDBUILT  TEST_EXEC ValidateJson_t 
  DATAFILES 
  TEST_ARGS  )
