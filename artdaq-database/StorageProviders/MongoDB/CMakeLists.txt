find_ups_product( mongodb ${mongodb_product_version} )

include_directories($ENV{MONGODB_INC}/bsoncxx)
include_directories($ENV{MONGODB_INC}/mongocxx)

find_library(BSONCXXLIB NAMES bsoncxx HINTS $ENV{MONGODB_LIB} )
find_library(MONGOCXXLIB NAMES mongocxx HINTS $ENV{MONGODB_LIB} )

             
art_make(
  LIB_LIBRARIES
  artdaq-database_BasicTypes
  ${BSONCXXLIB}
  ${MONGOCXXLIB}
  ${CETLIB}
)

install_headers()
install_source()
