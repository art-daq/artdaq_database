find_ups_product( mongodb ${mongodb_product_version} )

include_directories($ENV{MONGODB_INC}/bsoncxx)
find_library(BSONCXXLIB NAMES bsoncxx HINTS $ENV{MONGODB_LIB} )

art_make(
  LIB_LIBRARIES
  artdaq-database_DataFormats_common
  artdaq-database_DataFormats_Json
  artdaq-database_StorageProviders_MongoDB  
  ${BSONCXXLIB}
  ${CETLIB}  
  )

install_headers()
install_source()
