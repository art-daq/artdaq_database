cet_make(
  LIBRARIES
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}      
  ${CETLIB_EXCEPT}
  -ldl
  )

install_headers()
install_source()
