find_ups_product( mongodb v3_0_7 )

include_directories($ENV{MONGODB_FQ_DIR}/include/bsoncxx/v0.3)
include_directories($ENV{MONGODB_FQ_DIR}/include/mongocxx/v0.3)

art_make(
  LIB_LIBRARIES
  artdaq-database_FhiclJson
  ${FHICLCPP}
  ${CETLIB}
  )

art_make_exec(MongoDB_t
  LIBRARIES
  artdaq-database_FhiclJson
  ${FHICLCPP}
  ${CETLIB}
) 

install_headers()
install_source()
