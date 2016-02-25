art_make(
  LIB_LIBRARIES
  artdaq-database_BasicTypes
  ${BSONCXXLIB}
  ${MONGOCXXLIB}
  ${CETLIB}
  ${Boost_FILESYSTEM_LIBRARY}    
  ${Boost_SYSTEM_LIBRARY}    
)

install_headers()
install_source()
