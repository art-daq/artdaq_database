find_ups_product( mongodb v3_0_7 )

include_directories($ENV{MONGODB_FQ_DIR}/include/bsoncxx/v0.3)

find_library(BSONCXXLIB NAMES bsoncxx HINTS $ENV{MONGODB_FQ_DIR}/lib )

art_make(
  LIB_LIBRARIES
  ${BSONCXXLIB}
  ${CETLIB}  
  )

install_headers()
install_source()
