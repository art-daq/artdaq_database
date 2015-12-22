find_ups_product( mongodb v3_0_7 )

include_directories($ENV{MONGODB_FQ_DIR}/include/bsoncxx/v0.3)
include_directories($ENV{MONGODB_FQ_DIR}/include/mongocxx/v0.3)

find_library(BSONCXXLIB NAMES bsoncxx HINTS $ENV{MONGODB_FQ_DIR}/lib )
find_library(MONGOCXXLIB NAMES mongocxx HINTS $ENV{MONGODB_FQ_DIR}/lib )

             
art_make(
  LIB_LIBRARIES
  artdaq-database_BasicTypes
  ${BSONCXXLIB}
  ${MONGOCXXLIB}
  ${CETLIB}
)

install_headers()
install_source()
