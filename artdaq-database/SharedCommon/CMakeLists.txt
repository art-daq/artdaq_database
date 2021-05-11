cet_make(
  LIBRARIES
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}      
  $<TARGET_FILE:${CETLIB_EXCEPT}>
  -ldl
  )
  add_dependencies(artdaq-database_SharedCommon ${CETLIB_EXCEPT})

install_headers()
install_source()
