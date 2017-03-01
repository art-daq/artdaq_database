cet_make(
  LIBRARIES
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}    
  ${CETLIB}
  ${CETLIB_EXCEPT}
  )

install_headers()
install_source()
